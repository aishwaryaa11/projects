import java.util.ArrayList;

/**
 * An abstract class for representing coin game strategies.
 */
public abstract class CoinGameStrategy {

    /**
     * Given a game board, returns the set of possible moves this strategy
     * is allowed to make.
     *
     * @param game the game to make a move in
     * @return the set of possible moves
     */
    public abstract ArrayList<int[]> getMoves(CoinGame game);

    /**
     * Determines if there is a move this strategy can make
     * that results in game over
     *
     * @return true if the above statement holds
     */
    public boolean result(CoinGame game) {
        //for all moves this strategy can make on given game board
        boolean res = false;
        ArrayList<int[]> moves = this.getMoves(game);
        for (int i = 0; i < moves.size(); i++) {
            // moves[0] corresponds to coinIndex
            // moves[1] corresponds to new Index.

            if (game.posnHasGoal(moves.get(i)[1])) {
                if (game.goalsLeft() == 1)
                    res = true;
                else continue;
            }
        }
        return res;
    }

    /**
     * Predicts the move that will allow this strategy to win.
     *
     * @param game    the game to make a move on
     * @param players the other strategies competing against this one
     * @return the 'best' move for this strategy
     */
    public int[] learn(CoinGame game, CoinGameStrategy... players) {
        int[] bestmove = null;
        ArrayList<int[]> moves = this.getMoves(game);
        ArrayList<int[]> bestmoves = new ArrayList<>();
        //for every move this strategy can make
        for (int i = 0; i < moves.size(); i++) {
            //for every other player:
            for (CoinGameStrategy strat : players) {
                if (strat.equals(this))
                    continue;
                else {
                    //check if strat can make a move that results in game over
                    if (strat.result(game))
                        break;
                    else
                        bestmoves.add(moves.get(i));
                }
            }
        }
        int coinIdx = game.coinCount() - game.goalsLeft();
        if (coinIdx > 0) {
            for (int j = 1; j < moves.size(); j++) {
                if ((moves.get(moves.size() - j))[0] == coinIdx) {
                    bestmove= (moves.get(moves.size() - j));
                    break;
                }
            }
        }
        for (int j=0;j<bestmoves.size();j++){
            if (game.posnHasGoal(bestmoves.get(j)[1]))
                bestmove = bestmoves.get(j);
        }

        if (bestmove == null)
            bestmove = moves.get((int) (Math.random() * moves.size()));
        return bestmove;
    }

    /**
     * Make one move in the given name, according to some strategy.
     * <p>
     * PRECONDITIONS: {@code !game.isGameOver()}
     *
     * @param game the game to make a move on
     * @throws IllegalStateException if the game is over
     */
    public void playOneMove(CoinGame game, CoinGameStrategy... players){
        if (game.isGameOver())
            throw new IllegalStateException("Game Over.");
        int[] bestmove = this.learn(game, players);
        game.move(bestmove[0], bestmove[1]);
    }

}