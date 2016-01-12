import org.junit.Test;

import java.util.ArrayList;

import static org.junit.Assert.*;

public class StrategyThreeTest {
    String board = "O---Gn--O-G";
    CoinGame game = new StrictCoinGame(board);
    CoinGameStrategy strat3 = new StrategyThree();

    @Test
    public void testGetMoves() throws Exception {
        ArrayList<int[]> moves = strat3.getMoves(game);
        assertTrue(moves.size() == 11);

        int[] move1 = moves.get(0);
        assertTrue(move1[0] == 0);
        assertTrue(move1[1] == 1); //move 1 step to right

        int[] move2 = moves.get(1);
        assertTrue(move2[0] == 0);
        assertTrue(move2[1] == 6); //move 1 step down

        int[] move3 = moves.get(2);
        assertTrue(move3[0] == 0);
        assertTrue(move3[1] == 2); //move 2 steps to the right

        int[] move4 = moves.get(3);
        assertTrue(move4[0] == 1);
        assertTrue(move4[1] == 9); //move 1 step to the right

        int[] move5 = moves.get(4);
        assertTrue(move5[0] == 1);
        assertTrue(move5[1] == 7); //move 1 step to the left

        int[] move6 = moves.get(5);
        assertTrue(move6[0] == 1);
        assertTrue(move6[1] == 2); //move 1 step up

        int[] move7 = moves.get(6);
        assertTrue(move7[0] == 1);
        assertTrue(move7[1] == 10); //move 2 steps to the right

        int[] move8 = moves.get(7);
        assertTrue(move8[0] == 1);
        assertTrue(move8[1] == 6); //move 2 steps to the left

        int[] move9 = moves.get(8);
        assertTrue(move9[0] == 0);
        assertTrue(move9[1] == 7); //move 1 step down right

        int[] move10 = moves.get(9);
        assertTrue(move10[0] == 1);
        assertTrue(move10[1] == 3); //move 1 step up right

        int[] move11 = moves.get(10);
        assertTrue(move11[0] == 1);
        assertTrue(move11[1] == 1); //move 1 step up left
    }
}