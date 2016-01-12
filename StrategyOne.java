import java.util.ArrayList;

/**
 * Extends the abstract CoinGameStrategy class to implement only moves that
 * shift a coin either horizontally or vertically.
 */
public class StrategyOne extends CoinGameStrategy {
    /**
     * Constructs a StrategyOne for a single machine.
     */
    public StrategyOne() {}

    /**
     * Given a game board, returns the set of possible moves this strategy
     * is allowed to make ->move horizontally or vertically
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
                if ((posn+stepsize+1)%(game.boardDim()[0] +1) != 0){
                    moves.add(idx, new int[] {i, posn+stepsize});//right
                    idx = idx+1;
                }
                if (posn%(game.boardDim()[0] +1) !=0){
                    moves.add(idx, new int[] {i, posn-stepsize});//left
                    idx = idx+1;
                }
                if (posn+(stepsize*(game.boardDim()[0]+1)) < game.boardSize()){
                    int newdown = posn+(stepsize*(game.boardDim()[0]+1));
                    moves.add(idx, new int[] {i, newdown});//down
                    idx = idx+1;
                }
                if (posn-(stepsize*(game.boardDim()[0]+1)) >= 0){
                    int newup = posn-(stepsize*(game.boardDim()[0]+1));
                    moves.add(idx, new int[] {i, newup});//up
                    idx = idx+1;
                }
                stepsize = stepsize+1;
            }
        }
        return moves;
    }

    
}
