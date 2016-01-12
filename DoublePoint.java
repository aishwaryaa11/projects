/**
 * Creates a new instance of a Point with type Integer.
 */

public final class DoublePoint implements Point<Double> {
    private double x, y;

    /**
     * Constructor for Double Point.
     * @param x
     * @param y
     */
    private DoublePoint(double x, double y){
        this.x = x;
        this.y = y;
    }

    /**
     * Public static constructor for DoublePoint.
     * @param x
     * @param y
     * @return a new Double Point
     */
    public static Point<Double> point(double x, double y){
        return new DoublePoint(x,y);
    }

    @Override
    public double x(){
        return x;
    }

    @Override
    public double y(){
        return y;
    }

    @Override
    public int intX(){
        return (int)x();
    }

    @Override
    public int intY(){
        return (int)y();
    }

    @Override
    public Double boxedX(){
        return new Double(x());
    }

    @Override
    public Double boxedY(){
        return new Double(y());
    }

    @Override
    public Point<Double> scaleBy(double factor){
        return DoublePoint.point(Math.round(x() * factor), Math.round(y() * factor));
    }

    @Override
    public Point<Double> add(Point<?> other){
        return DoublePoint.point(x() + other.x(), y() + other.y());
    }

    @Override
    public Point<Double> subtract(Point<?> other){
        return DoublePoint.point(x() - other.x(), y() - other.y());
    }

    @Override
    public Point<Double> toDoublePoint(){
        return this;
    }

    @Override
    public Point<Integer> toIntegerPoint(){
        return IntegerPoint.point(intX(),intY());
    }

    @Override
    public boolean equals(double x, double y) {
        return (x == x()) && (y == y());
    }

    @Override
    public int hashCodeForDoublePoint() {
        return 31 * Double.hashCode(x()) + Double.hashCode(y());
    }

    @Override
    public Point<Double> interpolate(double weight, Point<?> other){
        double x_new = (1.0 - weight)*x() + weight*other.x();
        double y_new = (1.0 - weight)*y() + weight*other.y();
        return DoublePoint.point(x_new, y_new);
    }

    @Override
    public BoundingBox getBoundingBox(){
        return BoundingBox.fromTBLR(y(),y(),x(),x());
    }
}
