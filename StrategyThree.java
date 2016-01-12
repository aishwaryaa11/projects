import java.util.ArrayList;

/**
 * Extends the abstract CoinGameStrategy class to implement moves that
 * are a combination of StrategyOne and StrategyTwo.
 */
public class StrategyThree extends CoinGameStrategy{
    /**
     * Constructs a StrategyThree for a single machine.
     */
    public StrategyThree() {}

    /**
     * Given a game board, returns the set of possible moves this strategy
     * is allowed to make ->move horizontally, vertically or diagonally
     * @param game the game to make a move in
     * @return the set of possible moves
     */
    @Override
    public ArrayList<int[]> getMoves(CoinGame game) {
        ArrayList<int[]> moves1 = new StrategyOne().getMoves(game);
        ArrayList<int[]> moves2 = new StrategyTwo().getMoves(game);
        ArrayList<int[]> moves = new ArrayList<>();
        moves.addAll(moves1);
        moves.addAll(moves2);
        return moves;
    }
}
