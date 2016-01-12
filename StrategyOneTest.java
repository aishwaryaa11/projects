import org.junit.Test;

import java.util.ArrayList;

import static org.junit.Assert.*;

public class StrategyOneTest {
    String board = "O---Gn--O-G";
    CoinGame game = new StrictCoinGame(board);
    CoinGameStrategy strat1 = new StrategyOne();

    @Test
    public void testGetMoves() throws Exception {
        ArrayList<int[]> moves = strat1.getMoves(game);
        assertTrue(moves.size() == 8);

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
    }

    @Test
    public void testResult() throws Exception {
        assertFalse(strat1.result(game));
        game.move(0,2);
        int newposn = game.getCoinPosition(0);
        assertTrue(newposn == 2);

        //check dimensions of board
        int width = game.boardDim()[0];
        int height = game.boardDim()[1];
        assertTrue(width == 5);
        assertTrue(height == 2);

        game.move(0, 4);
        newposn = game.getCoinPosition(0);
        assertTrue(newposn == 4);

        assertTrue(strat1.result(game));
    }

    @Test
    public void testLearn() throws Exception {
        int[] bestmove = strat1.learn(game, strat1);
        assertTrue(bestmove[0] == 0);
        assertTrue(bestmove[1] == 1);
    }

    @Test
    public void testPlayOneMove() throws Exception {
        strat1.playOneMove(game, strat1);
        int newPosn = game.getCoinPosition(0);
        assertTrue(newPosn == 1);
        strat1.playOneMove(game, strat1);
        newPosn = game.getCoinPosition(0);
        assertTrue(newPosn == 2);

        game.move(0,4);
        strat1.playOneMove(game, strat1);
        newPosn = game.getCoinPosition(1);
        assertTrue(newPosn == 6);
        assertTrue(game.getCoinPosition(0) ==4);
    }
}