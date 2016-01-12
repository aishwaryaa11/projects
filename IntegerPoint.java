/**
 * Creates a new instance of a Point with type Integer.
 */
public class IntegerPoint implements Point<Integer>{
    private double x, y;

    /**
     * Constructs a Integer Point.
     * @param x
     * @param y
     */
    private IntegerPoint(double x, double y){
        this.x = x;
        this.y = y;
    }

    /**
     * Public static constructors for IntegerPoint.
     * @param x
     * @param y
     * @return a new Double Point
     */
    public static Point<Integer> point(int x, int y){
        return new IntegerPoint(x,y);
    }

    public static Point<Integer> point(double x, double y){
        return new IntegerPoint((int)x,(int)y);
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
    public Integer boxedX(){
        return new Integer(intX());
    }

    @Override
    public Integer boxedY(){
        return new Integer(intY());
    }

    @Override
    public Point<Integer> scaleBy(double factor){
        return IntegerPoint.point((int)Math.round(x()*factor),
                (int)Math.round(y()*factor));
    }

    @Override
    public Point<Integer> add(Point<?> other){
        return IntegerPoint.point(intX() + other.intX(), intY() + other.intY());
    }

    @Override
    public Point<Integer> subtract(Point<?> other){
        return IntegerPoint.point(intX() - other.intX(), intY() - other.intY());
    }

    @Override
    public Point<Double> toDoublePoint(){
        return DoublePoint.point(x(),y());
    }

    @Override
    public Point<Integer> toIntegerPoint(){
        return this;
    }

    @Override
    public boolean equals(int x, int y) {
        return (x == x()) && (y == y());
    }

    @Override
    public int hashCodeForIntegerPoint() {
        return 31 * intX() + intY();
    }

    @Override
    public Point<Integer> interpolate(double weight, Point<?> other){
        int x_new = (int)((1.0 - weight)*x() + weight*other.x());
        int y_new = (int)((1.0 - weight)*y() + weight*other.y());
        return IntegerPoint.point(x_new, y_new);
    }

    @Override
    public BoundingBox getBoundingBox(){
        return BoundingBox.fromTBLR(y(),y(),x(),x());
    }
}
