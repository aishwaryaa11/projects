import org.junit.Test;

import java.util.ArrayList;
import java.util.Collection;

import static org.junit.Assert.*;

public class TestAll {
    String board = "O--Gn--O-nG---"; //4 x 3 board
    CoinGame game = new StrictCoinGame(board);
    CoinGameStrategy strat1 = new StrategyOne();
    CoinGameStrategy strat2 = new StrategyTwo();
    CoinGameStrategy strat3 = new StrategyThree();

    @Test
    public void testGetMoves() throws Exception {
        ArrayList<int[]> moves = strat3.getMoves(game);
        assertTrue(moves.size() == 15);

        //moves from strategy1
        int[] move1 = moves.get(0);
        assertTrue(move1[0] == 0);
        assertTrue(move1[1] == 1); //move 1 step to right

        int[] move2 = moves.get(1);
        assertTrue(move2[0] == 0);
        assertTrue(move2[1] == 5); //move 1 step down

        int[] move3 = moves.get(2);
        assertTrue(move3[0] == 0);
        assertTrue(move3[1] == 2); //move 2 steps to right

        int[] move4 = moves.get(3);
        assertTrue(move4[0] == 0);
        assertTrue(move4[1] == 10); //move 2 steps down

        int[] move5 = moves.get(4);
        assertTrue(move5[0] == 1);
        assertTrue(move5[1] == 8); //move 1 step to right

        int[] move6 = moves.get(5);
        assertTrue(move6[0] == 1);
        assertTrue(move6[1] == 6); //move 1 step to left

        int[] move7 = moves.get(6);
        assertTrue(move7[0] == 1);
        assertTrue(move7[1] == 12); //move 1 step down

        int[] move8 = moves.get(7);
        assertTrue(move8[0] == 1);
        assertTrue(move8[1] == 2); //move 1 step up

        int[] move9 = moves.get(8);
        assertTrue(move9[0] == 1);
        assertTrue(move9[1] == 5); //move 2 steps to left

        //moves from strategy2
        int[] move10 = moves.get(9);
        assertTrue(move10[0] == 0);
        assertTrue(move10[1] == 6); //move 1 step right down

        int[] move11 = moves.get(10);
        assertTrue(move11[0] == 0);
        assertTrue(move11[1] == 12); //move 2 steps right down

        int[] move12 = moves.get(11);
        assertTrue(move12[0] == 1);
        assertTrue(move12[1] == 13); //move 1 step right down

        int[] move13 = moves.get(12);
        assertTrue(move13[0] == 1);
        assertTrue(move13[1] == 3); //move 1 step right up

        int[] move14 = moves.get(13);
        assertTrue(move14[0] == 1);
        assertTrue(move14[1] == 11); //move 1 step left down

        int[] move15 = moves.get(14);
        assertTrue(move15[0] == 1);
        assertTrue(move15[1] == 1); //move 1 step left up
    }

    @Test
    public void testResult() throws Exception {
        assertFalse(strat1.result(game));

        int[] movedown = strat1.getMoves(game).get(3);
        game.move(movedown[0],movedown[1]);

        int newposn = game.getCoinPosition(1);
        assertTrue(newposn == 10);

        assertFalse(strat1.result(game));
        assertTrue(strat2.result(game));
    }

    @Test
    public void testLearn() throws Exception {
        int[] move1 = strat1.learn(game, strat1,strat2,strat3);
        assertTrue(move1[0] == 0);
        assertTrue(move1[1] == 10);

        game.move(move1[0],move1[1]);

        int[] move2 = strat2.learn(game,strat1,strat2,strat3);
        assertTrue(move2[0] == 0);
        assertTrue(move2[1] == 3);
        game.move(move2[0],move2[1]);

        assertTrue(game.isGameOver());
    }

    @Test
    public void testPlayOneMove() throws Exception {
        String bigboy = "O-----Gn" +
                        "-------n" +
                        "-O-O-G-n" +
                        "--O---Gn" +
                        "-G--O--n" +
                        "----G--";
        CoinGame finalgame = new StrictCoinGame(bigboy);
        assertTrue(bigboy.length() == 47);

        System.out.println("Coin indices are: ");
        int[] newposn = new int[finalgame.coinCount()];
        for (int a=0;a<finalgame.coinCount();a++){
            newposn[a] = finalgame.getCoinPosition(a);
            System.out.print(newposn[a]);
            System.out.println("  ");
        }

        int winner = 0;
        while (!finalgame.isGameOver()) {
            strat1.playOneMove(finalgame, strat1, strat2, strat3);
            System.out.println("New coin indices are: ");
            for (int a = 0; a < finalgame.coinCount(); a++) {
                newposn[a] = finalgame.getCoinPosition(a);
                System.out.print(newposn[a]);
                System.out.println("  ");
            }
            winner = winner+1;
        }

        while (!finalgame.isGameOver()) {
            strat2.playOneMove(finalgame, strat1, strat2, strat3);
            System.out.println("New coin indices are: ");
            for (int a = 0; a < finalgame.coinCount(); a++) {
                newposn[a] = finalgame.getCoinPosition(a);
                System.out.print(newposn[a]);
                System.out.println("  ");
            }
            winner = winner+1;
        }

        while (!finalgame.isGameOver()){
            strat3.playOneMove(finalgame, strat1,strat2,strat3);
            System.out.println("New coin indices are: ");
            for (int a=0;a<finalgame.coinCount();a++){
                newposn[a] = finalgame.getCoinPosition(a);
                System.out.print(newposn[a]);
                System.out.println("  ");
            }
            winner = winner+1;
        }
        winner = (winner-1)%3+1;
        System.out.println("Game Over. Winner is: Strategy"+winner);

    }
}