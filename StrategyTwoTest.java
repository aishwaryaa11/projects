import org.junit.Test;

import java.util.ArrayList;

import static org.junit.Assert.*;

public class StrategyTwoTest {
    String board = "O---Gn--O-G";
    CoinGame game = new StrictCoinGame(board);
    CoinGameStrategy strat2 = new StrategyTwo();

    Point p1 = IntegerPoint.point(0,0);
    Point p2 = IntegerPoint.point(0,3);
    Point p3 = IntegerPoint.point(2,2);

    Point g1 = IntegerPoint.point(1,0);
    Point g2 = IntegerPoint.point(3,0);
    Point g3 = IntegerPoint.point(3,2);

    ArrayList<Point> coinlist = new ArrayList<>();
    ArrayList<Point> goallist = new ArrayList<>();

    @Test
    public void testGetMoves() throws Exception {
        ArrayList<int[]> moves = strat2.getMoves(game);
        assertTrue(moves.size() == 3);

        int[] move1 = moves.get(0);
        assertTrue(move1[0] == 0);
        assertTrue(move1[1] == 7); //move 1 step down right

        int[] move2 = moves.get(1);
        assertTrue(move2[0] == 1);
        assertTrue(move2[1] == 3); //move 1 step up right

        int[] move3 = moves.get(2);
        assertTrue(move3[0] == 1);
        assertTrue(move3[1] == 1); //move 1 step up left

        coinlist.add(0,p1);
        coinlist.add(1,p2);
        coinlist.add(2,p3);

        goallist.add(0,g1);
        goallist.add(1,g2);
        goallist.add(2,g3);

        CoinGame game2 = new StrictCoinGame(4,4, coinlist, goallist);
        assertTrue(game2.getCoinPosition(0) == 0);
        assertTrue(game2.getCoinPosition(1) == 12);
        assertTrue(game2.getCoinPosition(2) == 15);
        ArrayList<int[]> moves2 = strat2.getMoves(game2);
        assertTrue(moves2.size() == 9);

        int[] move = moves2.get(0);
        assertTrue(move[0] == 0);
        assertTrue(move[1] == 6);

        move = moves2.get(1);
        assertTrue(move[0] == 0);
        assertTrue(move[1] == 12);

        move = moves2.get(2);
        assertTrue(move[0] == 1);
        assertTrue(move[1] == 18);

        move = moves2.get(3);
        assertTrue(move[0] == 1);
        assertTrue(move[1] == 8);

        move = moves2.get(4);
        assertTrue(move[0] == 1);
        assertTrue(move[1] == 16);

        move = moves2.get(5);
        assertTrue(move[0] == 1);
        assertTrue(move[1] == 6);

        move = moves2.get(6);
        assertTrue(move[0] == 1);
        assertTrue(move[1] == 0);

        move = moves2.get(7);
        assertTrue(move[0] == 2);
        assertTrue(move[1] == 11);

        move = moves2.get(8);
        assertTrue(move[0] == 2);
        assertTrue(move[1] == 7);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testException1(){
        coinlist.add(0,p1);
        coinlist.add(1,p2);

        goallist.add(0,g1);
        goallist.add(1,g2);
        goallist.add(2,g3);

        CoinGame gameExp1 = new StrictCoinGame(4,4, coinlist, goallist);
    }
    @Test(expected = IllegalArgumentException.class)
    public void testException2(){
        Point p4 = IntegerPoint.point(5,1);
        coinlist.add(0,p4);
        goallist.add(0,g1);
        CoinGame gameExp2 = new StrictCoinGame(4,4, coinlist, goallist);
    }
    @Test(expected = IllegalArgumentException.class)
    public void testException3(){
        Point p4 = IntegerPoint.point(5,1);
        coinlist.add(0,p1);
        coinlist.add(1,p2);

        goallist.add(0,g1);
        goallist.add(1,p4);
        CoinGame gameExp2 = new StrictCoinGame(4,4, coinlist, goallist);
    }
}