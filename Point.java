/**
 * An interface to immutable 2-D points and vectors. We use a single interface
 * for continuous (real-value) and discrete (integer-valued) points, which
 * allows us to write generic code over points of both varieties.
 * <p>
 * Type parameter {@code <T>} specifies whether the concrete implementation
 * of this interface uses {@code double}s or {@code int}s to represent the
 * coordinates. In particular:
 * <ul>
 *   <li>Implementations of {@code Point<Double>} must store the coordinates
 * as {@code double}s, such that methods {@link #x()} and {@link #y()} need not
 * perform conversions.</li>
 *   <li>Implementations of {@code Point<Integer>} must store the coordinates as
 * {@code int}s, such that methods {@link #intX()} and {@link #intY()} need not
 * perform conversions.</li>
 * </ul>
 * <p>
 * An additional requirement of this interface is that {@code Point<Integer>}s
 * are never equal to {@code Point<Double>}s. However, two {@code
 * Point<Integer>}s with the same coordinates must be considered equal even if
 * they are represented by different classes, and likewise for {@code
 * Point<Double>}s. Furthermore, because {@link Object#hashCode()} is required
 * to return the same value for objects that are considered equal, this means
 * that all implementations of {@code Point<Integer>} must agree on their {@code
 * hashCode()} methods (and likewise all {@code Point<Double>}s must agree with
 * one another). In order to help with this, this interface provides default
 * implementations for methods {@link #hashCodeForIntegerPoint()} and {@link
 * #hashCodeForDoublePoint()}. Implementations of the {@code Point} interface
 * should override {@code hashCode()} to return the value of one of these two.
 *
 * @param <T> (boxed version of) the coordinate representation type
 */
public interface Point<T> extends Boundable, Interpolable<Point<?>> {
  /**
   * Gets the x coordinate of this point as a {@code double}.
   *
   * @return x
   */
  double x();

  /**
   * Gets the y coordinate of this point as a {@code double}.
   *
   * @return y
   */
  double y();

  /**
   * Gets the x coordinate of this point as a {@code int}.
   *
   * @return x
   */
  int intX();

  /**
   * Gets the y coordinate of this point as a {@code int}.
   *
   * @return y
   */
  int intY();

  /**
   * Gets the x coordinate of this point as a boxed numeric value.
   *
   * @return x
   */
  T boxedX();

  /**
   * Gets the y coordinate of this point as a boxed numeric value.
   *
   * @return y
   */
  T boxedY();

  /*
   * VECTOR OPERATIONS
   */

  /**
   * Scales this vector by the given factor, returning the scaled vector.
   * <p>
   * This operation treats points as 2-D vectors. For integral vectors, this
   * operation is not closed and will require rounding. The multiplication
   * should be performed with {@code double}s, and the result rounded with
   * {@link Math#round(double)}.
   *
   * @param factor the factor to scale by
   * @return {@code (factor * x, factor * y)}
   */
  Point<T> scaleBy(double factor);

  /**
   * Adds this vector to another vector, returning the sum.
   * (This operation treats points as 2-D vectors.)
   *
   * @param other the other vector
   * @return {@code (x + other.x, y + other.y)}
   */
  Point<T> add(Point<?> other);

  /**
   * Subtracts another vector from this vector, returning the difference. (This
   * operation treats points as 2-D vectors.)
   *
   * @param other the other vector
   * @return {@code (x - other.x, y - other.y)}
   */
  Point<T> subtract(Point<?> other);

  /**
   * Returns the magnitude of this vector. (This operation treats points as
   * 2-D vectors.)
   *
   * @return the magnitude
   */
  default double magnitude() {
    return Math.hypot(y(), x());
  }

  /**
   * Returns the angle of this vector in the positive direction from the
   * positive x axis. This operation treats points as 2-D vectors.
   *
   * @return -<em>pi</em> ≤ <em>theta</em> &lt; <em>pi</em>
   */
  default double angle() {
    return Math.atan2(y(), x());
  }

  /**
   * Returns a {@code double} point equivalent to this point. If this point
   * <em>is</em> a {@code double} point, then it may return {@code this};
   * otherwise it may have to construct a new point to perform the conversion.
   *
   * @return {@code this} as a {@code Point<Double>}
   */
  Point<Double> toDoublePoint();

  /**
   * Returns a {@code int} point equivalent to this point. If this point
   * <em>is</em> an {@code int} point, then it may return {@code this};
   * otherwise it may have to construct a new point to perform the conversion.
   *
   * @return {@code this} as a {@code Point<Integer>}
   */
  Point<Integer> toIntegerPoint();

  /**
   * Determines whether this point is equal to the {@code int} point with the
   * given coordinates. This is to implement double dispatch for point
   * equality, so that {@code Point<Integer>}s are never equal to {@code
   * Point<Double>}s, but different implementations of {@code
   * Point<Integer>} are equal if their coordinates are.
   * <p>
   * The default implementation of this method returns {@code false},
   * so implementations of {@code Point<Integer>} must override this method
   * to actually perform a comparison.
   *
   * @param x the x coordinate
   * @param y the y coordinate
   * @return whether {@code this} is a {@code Point<Integer>} equal to
   *         ({@code x}, {@code y})
   * @see #hashCodeForIntegerPoint()
   * @see #equals(double, double)
   */
  default boolean equals(int x, int y) {
    return false;
  }

  /**
   * Determines whether this point is equal to the {@code double} point with
   * the given coordinates. This is to implement double dispatch for point
   * equality, so that {@code Point<Double>}s are never equal to {@code
   * Point<Integer>}s, but different implementations of {@code
   * Point<Double>} are equal if their coordinates are.
   * <p>
   * The default implementation of this method returns {@code false},
   * so implementations of {@code Point<Double>} must override this method
   * to actually perform a comparison.
   *
   * @param x the x coordinate
   * @param y the y coordinate
   * @return whether {@code this} is a {@code Point<Double>} equal to
   *         ({@code x}, {@code y})
   * @see #hashCodeForDoublePoint()
   * @see #equals(double, double)
   */
  default boolean equals(double x, double y) {
    return false;
  }

  /**
   * Computes the hash code that all {@code Point<Integer>} implementations
   * are required to return from their {@code hashCode()} methods.
   * Implementations of {@code Point<Integer>} should override {@link
   * Object#hashCode()} and have it delegate to this method.
   *
   * @return the correct value for hashing an {@code int} point
   * @see #equals(int, int)
   * @see Object#hashCode()
   */
  default int hashCodeForIntegerPoint() {
    return 31 * intX() + intY();
  }

  /**
   * Computes the hash code that all {@code Point<Double>} implementations
   * are required to return from their {@code hashCode()} methods.
   * Implementations of {@code Point<Double>} should override {@link
   * Object#hashCode()} and have it delegate to this method.
   *
   * @return the correct value for hashing an {@code double} point
   * @see #equals(double, double)
   * @see Object#hashCode()
   */
  default int hashCodeForDoublePoint() {
    return 31 * Double.hashCode(x()) + Double.hashCode(y());
  }

  /**
   * @inheritDoc
   *
   * This method is overridden to refine its return type to {@code
   * Point<T>} from {@code Point<?>}
   *
   * @return The interpolated {@code Point<T>}
   */
  @Override
  Point<T> interpolate(double weight, Point<?> other);
}
