import java.util.ArrayList;

/**
 * Extends the abstract CoinGameStrategy class to implement only moves that
 * shift a coin diagonally.
 */
public class StrategyTwo extends CoinGameStrategy {
    /**
     * Constructs a StrategyTwo for a single machine.
     */
    public StrategyTwo() {}

    /**
     * Given a game board, returns the set of possible moves this strategy
     * is allowed to make ->move diagonally
     * @param game the game to make a move in
     * @return the set of possible moves
     */
    @Override
    public ArrayList<int[]> getMoves(CoinGame game){
        ArrayList<int[]> moves = new ArrayList<>();
        int idx = 0;
        for (int i=0;i<game.coinCount();i++){ // for each coin
            int posn = game.getCoinPosition(i);
            int stepsize = 1;

            while (stepsize < 3){//strategy can only move 1 or 2 steps per move
                int newrd = posn+(stepsize*(game.boardDim()[0] +2));
                if ((posn <= game.boardSize()-
                        (stepsize*(game.boardDim()[0]+2))) &&
                        ((posn+stepsize+1)%(game.boardDim()[0] +1) != 0) &&
                        (newrd >=0)){

                    moves.add(idx, new int[] {i, newrd});//right-down
                    idx = idx+1;
                }
                int newru = posn-(stepsize*game.boardDim()[0]);
                if ((posn >= (game.boardDim()[0] +1)*stepsize) &&
                        (posn <= (game.boardSize()-stepsize)) &&
                        ((posn+stepsize+1)%(game.boardDim()[0] +1) != 0) &&
                        (newru>=0)){

                    moves.add(idx, new int[] {i, newru});//right-up
                    idx = idx+1;
                }
                int newld = posn+(stepsize*game.boardDim()[0]);
                if ((posn <= (game.boardSize()-
                        stepsize*(game.boardDim()[0]+1))) &&
                        ((posn-stepsize+1)%(game.boardDim()[0] +1) != 0) &&
                        (posn%(game.boardDim()[0] +1) != 0) && (newld>=0)){

                    moves.add(idx, new int[] {i, newld});//left-down
                    idx = idx+1;
                }
                int newlu = posn-(stepsize*(game.boardDim()[0]+2));
                if ((posn >= (game.boardSize()-
                        stepsize*(game.boardDim()[0]+game.boardDim()[1]))) &&
                        ((posn-stepsize+1)%(game.boardDim()[0] +1) != 0) &&
                        (posn%(game.boardDim()[0] +1) != 0) && (newlu>=0)){

                    moves.add(idx, new int[] {i, newlu});//left-up
                    idx = idx+1;
                }
                stepsize = stepsize+1;
            }
        }
        return moves;
    }

}
