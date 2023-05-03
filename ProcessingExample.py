# Import necessary Python libraries
import DataProcessingLib as lib
from scipy.io import wavfile

# Get data for plotting
lib.convertAll('folder name', 44100)  # convert all files in the folder to .wav
fs, data = wavfile.read('filename.wav')  # read .wav file into sampling frequency and data points
data2 = lib.bandpass(data, 1000, 3000, fs)  # apply bandpass filter from 1000-3000 Hz (example)

# Plot data
lib.waveform(data, fs, title='Unfiltered')  # plot waveform of unfiltered data
lib.waveform(data2, fs,  title='Band-Pass')  # plot waveform of filtered data
lib.spectrogram(data, fs, title='Unfiltered')  # plot spectrogram of unfiltered data
lib.spectrogram(data2, fs, title='Band-Pass')  # plot spectrogram of filtered data
lib.fft(data, fs, title='Unfiltered')  # plot FFT of unfiltered data
lib.fft(data2, fs, title='Band-Pass')  # plot FFT of filtered data

