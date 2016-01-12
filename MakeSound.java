/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package soundcheck;

import java.io.File;
import java.io.IOException;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;
import java.util.*;

/**
 *
 * @author frotein
 */
public class MakeSound {
/**
     * @param args the command line arguments
     */
     private final int BUFFER_SIZE = 512000;
    private File soundFile;
    private AudioInputStream audioStream;
    private AudioFormat audioFormat;
    private SourceDataLine sourceLine;

    /**
     * @param filename the name of the file that is going to be played
     */
        
public List<Double> playSound(String filename){

        String strFilename = filename;

        try {
            soundFile = new File(strFilename);
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        }

        try {
            audioStream = AudioSystem.getAudioInputStream(soundFile);
        } catch (Exception e){
            e.printStackTrace();
            System.exit(1);
        }

        audioFormat = audioStream.getFormat();

        DataLine.Info info = new DataLine.Info(SourceDataLine.class, audioFormat);
        try {
            sourceLine = (SourceDataLine) AudioSystem.getLine(info);
            sourceLine.open(audioFormat);
        } catch (LineUnavailableException e) {
            e.printStackTrace();
            System.exit(1);
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        }

        sourceLine.start();

        int nBytesRead = 0;
        byte[] abData = new byte[BUFFER_SIZE];
        while (nBytesRead != -1) {
            try {
                nBytesRead = audioStream.read(abData, 0, abData.length);
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (nBytesRead >= 0) {
                @SuppressWarnings("unused")
                int nBytesWritten = sourceLine.write(abData, 0, nBytesRead);
            }
        }

        sourceLine.drain();
        sourceLine.close();
        
        //Calculate the sample rate
float sample_rate = audioFormat.getSampleRate() / 250;
System.out.println("sample rate = "+sample_rate);

//Calculate the length in seconds of the sample
float T = audioStream.getFrameLength() / audioFormat.getFrameRate();
//System.out.println("T = "+T+ " (length of sampled sound in seconds)");

//Calculate the number of equidistant points in time
int n = (int) (T * sample_rate) / 2;
System.out.println("n = "+n+" (number of equidistant points)");

//Calculate the time interval at each equidistant point
float h = (T / n);
System.out.println("h = "+h+" (length of each time interval in seconds)");

//Determine the original Endian encoding format
boolean isBigEndian = audioFormat.isBigEndian();

//this array is the value of the signal at time i*h
int x[] = new int[n];

//convert each pair of byte values from the byte array to an Endian value
for (int i = 0; i < n*2; i+=2) {
	int b1 = abData[i];
	int b2 = abData[i + 1];
	if (b1 < 0) b1 += 0x100;
	if (b2 < 0) b2 += 0x100;

	int value;

	//Store the data based on the original Endian encoding format
	if (!isBigEndian) value = (b1 << 8) + b2;
	else value = b1 + (b2 << 8);
	x[i/2] = value;
}

//do the DFT for each value of x sub j and store as f sub j
double f[] = new double[n/2];
double amp1 = 0,amp2 = 0,amp3 = 0;
double freq1 = 0,freq2 = 0,freq3 = 0;
  List<Double> list = new ArrayList<Double>();
for (int j = 0; j < n/2; j++) {

	double firstSummation = 0;
	double secondSummation = 0;

	for (int k = 0; k < n; k++) {
     		double twoPInjk = ((2 * Math.PI) / n) * (j * k);
     		firstSummation +=  x[k] * Math.cos(twoPInjk);
     		secondSummation += x[k] * Math.sin(twoPInjk);
	}

        f[j] = Math.abs( Math.sqrt(Math.pow(firstSummation,2) + 
        Math.pow(secondSummation,2)) );
        
	double amplitude = 2 * f[j]/n;
	double frequency = j * h / T * sample_rate;
	freq3 = freq2;
        freq2 = freq1;
        freq1 = frequency;
        amp3 = amp2;
        amp2 = amp1;
        amp1 = amplitude;
        if(amp2 > amp1 && amp2 > amp3)
        {
            list.add(freq2);
            
        }
        
       System.out.println("frequency = "+frequency+", amp = "+amplitude);
        
}

return list;
    }
    
    static void dft(double[] inreal, double[] inimag, double[] outreal, double[] outimag) {
    int n = inreal.length;
    for (int k = 0; k < n; k++) {  // For each output element
        double sumreal = 0;
        double sumimag = 0;
        for (int t = 0; t < n; t++) {  // For each input element
            double angle = 2 * Math.PI * t * k / n;
            sumreal +=  inreal[t] * Math.cos(angle) + inimag[t] * Math.sin(angle);
            sumimag += -inreal[t] * Math.sin(angle) + inimag[t] * Math.cos(angle);
        }
        outreal[k] = sumreal;
        outimag[k] = sumimag;
    }
    
    }
}
