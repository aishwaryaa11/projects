/**
 * Interface for types that support weighted averages. This can be used to
 * compute a smooth change between two values. For example,
 * interpolating between two points from weight 0 to weight 1 will trace out
 * the line segment connecting the two points.
 */
public interface Interpolable<T> {
  /**
   * Interpolates a value between {@code this} and {@code other}. The {@code
   * weight} parameter controls the weight of the average: 0.0 returns
   * {@code this}, 1.0 returns {@code other}, and weights in between result
   * in some linear interpolation of the two.
   *
   * <p>
   * When interpolating numbers, this is merely a weighted average,
   * where {@code weight} is the weight of {@code other} and {@code 1 - weight}
   * is the weight of {@code this}:
   * <pre><code>    (1.0 - weight) * this + weight * other</code></pre>
   * The purpose of this interface is to generalize this concept beyond
   * numbers. For example, for vectors, we can interpolate by interpolating
   * the components pairwise. In other domains, the operation may be defined
   * differently; for example, color interpolation can treat the red, green,
   * and blue components as an ordinary vector, but alpha needs to be treated
   * specially.
   *
   * <p><strong>Precondition:
   *   </strong>0.0 ≤ {@code weight} ≤ 1.0
   *
   * @param weight the proportion of the distance to go from {@code this} to
   *               {@code other}
   * @param other  the other value
   * @return the interpolated value
   */
  T interpolate(double weight, T other);

  /**
   * Interpolates between two {@code double}s. Parameter {@code weight} is
   * the weight of {@code b}, which means that {@code 1 - weight} is the
   * weight of {@code a}. Note that weights outside the unit interval will
   * “exterpolate” points along the same line, but not between {@code a} and
   * {@code b}.
   *
   * @param a the result when {@code weight == 0.0}
   * @param weight the weight of {@code b}
   * @param b the result when {@code weight == 1.0}
   * @return {@code (1 - weight) * a + weight * b}
   */
  static double interpolate(double a, double weight, double b) {
    return (1 - weight) * a + weight * b;
  }
}
