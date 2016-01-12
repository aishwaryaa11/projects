/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package soundcheck;

import java.util.*;

/**
 *
 * @author thurston
 */
public class SoundCheck {

    /**
     * @param args the command line arguments
     */
    static List<Double> l1,l2;
    public static void main(String[] args) {
       
        MakeSound sound = new MakeSound();
       
                
        for(int i =0; i< 2;i++)  
        {
        if(i == 0) 
        {
            System.out.println("compiling file 1");
            l1 = sound.playSound(args[i]);
        }
            if(i == 1)
            {
             System.out.println("compiling file 2");
            l2 = sound.playSound(args[i]);
            break;
            }
        }
        
        l1.removeAll(l2);
        if (l1.size() <= 50){
            System.out.println("MATCH " + args[2].toString() + " " + args[3].toString());
            System.out.println(Integer.toString(100-(50 / l1.size())) + "% Match");
        }
        else 
            System.out.println("NO MATCH " + args[2].toString() + " " + args[3].toString());
        }
    
}