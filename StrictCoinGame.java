/**
 * Created by aishwaryaafzulpurkar on 12/11/14.
 */
import java.util.*;

public class StrictCoinGame implements CoinGame{
    protected char[] boardChar;
    protected String board;
    protected int cc;

    public StrictCoinGame(String board){
        String board_tmp = board;
        board_tmp = board_tmp.replace("O", "");
        cc = board.length() - board_tmp.length();
        board_tmp = board_tmp.replace("-", "");
        board_tmp = board_tmp.replace("n", "");
        board_tmp = board_tmp.replace("G", "");
        if (board_tmp.length() != 0){
            throw new IllegalArgumentException("Invalid board configuration.");
        }
        this.board = board;
        char[] boardChar = board.toCharArray();
        this.boardChar = boardChar;
    }

    public StrictCoinGame(int x, int y,
                            ArrayList<Point> coinlist,
                            ArrayList<Point> goallist){
        cc = coinlist.size();
        if (cc != goallist.size()){ //if # of coins does not equal # of goals
            throw new IllegalArgumentException
                    ("Number of goals should equal number of coins.");
        }
        //if a point(x,y) in coinlist or goallist exceeds board dimensions
        for (int i=0;i<cc;i++){
            if ((coinlist.get(i).intX() >= x) ||
                    (coinlist.get(i).intY() >= y)){
                throw new IllegalArgumentException("Invalid coin indices.");
            }
            if ((goallist.get(i).intX() >= x) ||
                    (goallist.get(i).intY() >= y)){
                throw new IllegalArgumentException("Invalid goal indices.");
            }
        }

        boardChar = new char[(x+1)*y];
        Arrays.fill(boardChar, '-');
        for (int j=0; j<cc;j++){
            for (int i=0;i<boardChar.length;i++) {
                if ((i + 1) % (x + 1) == 0)
                    boardChar[i] = 'n';
                else if (i == coinlist.get(j).intX() + (coinlist.get(j).intY() * (x + 1)))
                    boardChar[i] = 'O';
                else if (i == goallist.get(j).intX() + (goallist.get(j).intY() * (x + 1)))
                    boardChar[i] = 'G';
            }
        }
        this.board = new String(boardChar);
    }

    /**
     * Gets the size of the board.
     * @return board size
     */
    @Override
    public int boardSize(){
        return board.length();
    }

    /**
     * Gets the dimensions of the 2D board.
     * @return board dimensions
     */
    @Override
    public int[] boardDim(){
        String boardtmp = board;
        int i = boardtmp.indexOf('n');
        int countn = boardtmp.length() - boardtmp.replace("n", "").length();
        int j = countn+1;
        return new int[] {i,j};
    }

    /**
     * Gets the number of coins on board.
     * @return number of coins
     */
    @Override
    public int coinCount(){
        return cc;
    }

    /**
     * Gets the position of a specified coin on the board.
     * @param coinIndex which coin to look up
     * Precondition: board.length > 0
     *
     * @return position on board of coin
     * returns -1 if no coin found
     */
    @Override
    public int getCoinPosition(int coinIndex){
        if (cc == 0)
            throw new IllegalArgumentException("No coins on board");
        else{
            int acc = -1; //index of last coin
            while (coinIndex != -1){
                acc = board.indexOf("O", acc+1);
                coinIndex = coinIndex-1;
            }
            return board.indexOf("O", acc);
        }
    }

    /**
     * Tells players if the game is over.
     * (If no moves are left, all 'G's are replaced by 'O's)
     * Precondition: board.length >0 and contains
     * no char other than 'O','-','n' or 'G'.
     *
     * @return true if game is over, else returns false
     * returns true if no coins are on board
     */
    @Override
    public boolean isGameOver(){
        return !(board.contains("G"));
    }

    @Override
    public int goalsLeft(){
        String boardtmp = board;
        int count = boardtmp.length() - boardtmp.replace("G", "").length();
        return count;
    }

    @Override
    public boolean posnHasGoal(int posn){
        return (board.charAt(posn) == 'G');
    }

    /**
     * Constructs a new coin game board with given coin in new position.
     * Precondition: board.length >0 and does not contain characters other than 'O' and '-'.
     * @param coinIndex   which coin to move
     * @param newPosition where to move it to
     */
    @Override
    public void move(int coinIndex, int newPosition){
        boardChar[getCoinPosition(coinIndex)] = '-';
        boardChar[newPosition] = 'O';
        board = new String(boardChar);
    }

    /**
     * Converts board from a char array format to string.
     * @return the modified board in string format
     */
    @Override
    public String toString(){
        board = new String(boardChar);
        return board;
    }

}
