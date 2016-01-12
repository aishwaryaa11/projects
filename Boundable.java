/**
 * Objects that are boundable know their bounding boxes. For objects with a
 * visual representation, the bounding box should contain their entire
 * non-transparent area.
 */
public interface Boundable {
  /**
   * Gets this object's bounding box.
   *
   * @return this object's bounding box
   */
  BoundingBox getBoundingBox();
}
