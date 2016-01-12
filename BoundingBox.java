import static java.lang.Double.isFinite;

/**
 * An interface for computing with bounding boxes. Also includes static factory
 * methods for creating bounding boxes.
 * <p>
 * A bounding box is a finite, rectangular area of the plane. We can specify a
 * bounding box by giving the x coordinates of the left and right sides, and the
 * y coordinates of the top and bottom sides. Or, we can specify one in terms of
 * two opposite corners.
 * <p>
 * Our bounding boxes are considered to include their boundaries.
 */
public interface BoundingBox extends Boundable, Interpolable<BoundingBox> {
  /**
   * Gets the y coordinate of the top of this bounding box.
   *
   * @return {@code topLeft.y()}
   */
  double top();

  /**
   * Gets the y coordinate of the bottom of this bounding box.
   *
   * @return {@code bottomRight.y()}
   */
  double bottom();

  /**
   * Gets the x coordinate of the left side of this bounding box.
   *
   * @return {@code topLeft.x()}
   */
  double left();

  /**
   * Gets the x coordinate of the right side of this bounding box.
   *
   * @return {@code bottomRight.x()}
   */
  double right();

  /**
   * Gets the width of the bounding box.
   *
   * @return {@code right() - left()}
   */
  default double width() {
    return right() - left();
  }

  /**
   * Gets the height of the bounding box.
   *
   * @return {@code bottom() - top()}
   */
  default double height() {
    return bottom() - top();
  }

  /**
   * Gets the upper-left corner of this bounding box.
   *
   * @return {@code point(left(), top())}
   */
  Point<Double> topLeft();

  /**
   * Gets the upper-right corner of this bounding box.
   *
   * @return {@code point(right(), top())}
   */
  Point<Double> topRight();

  /**
   * Gets the lower-left corner of this bounding box.
   *
   * @return {@code point(left(), bottom())}
   */
  Point<Double> bottomLeft();

  /**
   * Gets the lower-right corner of this bounding box.
   *
   * @return {@code point(right(), bottom())}
   */
  Point<Double> bottomRight();

  /**
   * Determines whether the bounding box of {@code other} is a subset of this
   * bounding box.
   *
   * @param other the other object
   * @return whether {@code other} fits within this
   */
  default boolean contains(Boundable other) {
    return contains(other.getBoundingBox());
  }

  /**
   * Determines whether the bounding box {@code other} is a subset of this
   * bounding box.
   *
   * @param other the other bounding box
   * @return whether {@code other} fits within this
   */
  default boolean contains(BoundingBox other) {
    return other.left() >= left()
            && other.right() <= right()
            && other.top() >= top()
            && other.bottom() <= bottom();
  }

  /**
   * Determines whether two bounding boxes are disjoint.
   *
   * @param other the other bounding box
   * @return whether the boxes are disjoint
   * @see #intersect(BoundingBox)
   */
  default boolean isDisjoint(BoundingBox other) {
    return other.right() <= left()
            || other.left() >= right()
            || other.bottom() <= top()
            || other.top() >= bottom();
  }

  /**
   * Joins this bounding box with another bounding box. Returns the smallest
   * bounding box containing both {@code this} and {@code other}.
   *
   * @param other the other bounding box
   * @return {@code BoundingBox.containing(this, other)}
   * @see #containing(Boundable, Boundable...)
   */
  default BoundingBox join(BoundingBox other) {
    return containing(this, other);
  }

  /**
   * Intersects this bounding box with another bounding box. Returns the
   * largest bounding box contained by both {@code this} and {@code other}.
   *
   * @param other the other bounding box
   * @return {@code BoundingBox.containedBy(this, other)}
   * @throws IllegalArgumentException if {@code !disjoint(other)}
   * @see #containedBy(Boundable, Boundable...)
   * @see #isDisjoint(BoundingBox)
   */
  default BoundingBox intersect(BoundingBox other) {
    return containedBy(this, other);
  }

  @Override
  default BoundingBox getBoundingBox() {
    return this;
  }

  @Override
  default public BoundingBox interpolate(double weight, BoundingBox other) {
    return fromTBLR(Interpolable.interpolate(top(), weight, other.top()),
                    Interpolable.interpolate(bottom(), weight, other.bottom()),
                    Interpolable.interpolate(left(), weight, other.left()),
                    Interpolable.interpolate(right(), weight, other.right()));
  }

  /**
   * Creates a bounding box with the given sides.
   *
   * @param top the y coordinate of the top of the bounding box
   * @param bottom the y coordinate of the bottom of the bounding box
   * @param left the x coordinate of the left side of the bounding box
   * @param right the x coordinate of the right side of the bounding box
   * @return the new bounding box
   * @throws IllegalArgumentException if {@code top > bottom} or
   *          {@code left > right}
   */
  public static BoundingBox fromTBLR(double top, double bottom,
                                     double left, double right)
  {
    if (top > bottom || left > right) {
      throw new IllegalArgumentException("bounding box must be non-empty");
    }

    if (! (isFinite(top) && isFinite(bottom)
            && isFinite(left) && isFinite (right)))
    {
      throw new IllegalArgumentException("bounding box must be finite");
    }

    return new BoundingBox() {
      @Override public double top() { return top; }
      @Override public double bottom() { return bottom; }
      @Override public double left() { return left; }
      @Override public double right() { return right; }

      @Override
      public Point<Double> topLeft() {
        return DoublePoint.point(left,top);
      }

      @Override
      public Point<Double> topRight() {
          return DoublePoint.point(right,top);
      }

      @Override
      public Point<Double> bottomLeft() {
          return DoublePoint.point(left,bottom);
      }

      @Override
      public Point<Double> bottomRight() {
          return DoublePoint.point(right,bottom);
      }
    };
  }

  /**
   * Returns the smallest bounding box containing all the arguments.
   *
   * @param first one object to bound
   * @param rest the other objects to bound
   * @return the smallest bounding box bounding {@code contents}
   * @see #join(BoundingBox)
   */
  public static BoundingBox containing(Boundable first, Boundable... rest) {
    BoundingBox bb = first.getBoundingBox();

    double left   = bb.left();
    double right  = bb.right();
    double top    = bb.top();
    double bottom = bb.bottom();

    for (Boundable item : rest) {
      bb = item.getBoundingBox();

      left   = Double.min(left, bb.left());
      right  = Double.max(right, bb.right());
      top    = Double.min(top, bb.top());
      bottom = Double.max(bottom, bb.bottom());
    }

    return fromTBLR(top, bottom, left, right);
  }

  /**
   * Returns the largest bounding box contained by all the arguments.
   *
   * @param first one object to use as a bound
   * @param rest the other objects to use as a bounds
   * @return the largest box bounded by {@code containers}
   * @throws IllegalArgumentException if the intersection is empty
   * @see #intersect(BoundingBox)
   */
  public static BoundingBox containedBy(Boundable first, Boundable... rest) {
    BoundingBox bb = first.getBoundingBox();

    double left   = bb.left();
    double right  = bb.right();
    double top    = bb.top();
    double bottom = bb.bottom();

    for (Boundable item : rest) {
      bb = item.getBoundingBox();

      left   = Double.max(left, bb.left());
      right  = Double.min(right, bb.right());
      top    = Double.max(top, bb.top());
      bottom = Double.min(bottom, bb.bottom());
    }

    try {
      return fromTBLR(top, bottom, left, right);
    } catch (IllegalArgumentException e) {
      throw new IllegalArgumentException("intersection is empty", e);
    }
  }
}


