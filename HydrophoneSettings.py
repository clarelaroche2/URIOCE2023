import tkinter as tk
from tkinter import ttk
from tkinter import filedialog as fd

'''Initial Window Setup'''
try:
    # Fixes blurred text on Windows, gets correct screen width and height
    from ctypes import windll

    windll.shcore.SetProcessDpiAwareness(2)
finally:
    root = tk.Tk()  # Creates application window
    root.title('Hydrophone Settings')  # Changes window title

'''Basic Window Attributes'''
wWind = 1000
hWind = 400
wScreen = root.winfo_screenwidth()
hScreen = root.winfo_screenheight()
wCenter = int((wScreen - wWind) / 2)
hCenter = int((hScreen - hWind) / 2)
root.geometry(f'{wWind}x{hWind}+{wCenter}+{hCenter}')
root.minsize(wWind, hWind)  # Sets the minimum window width and height
yPad = 5
xPad = 5
global directory

'''Widgets'''
# Sampling Frequency
'''
sfLabel = ttk.Label(root, text='Sampling Frequency (Hz):')
sfLabel.grid(row=0, column=0, sticky=tk.NW, padx=xPad, pady=yPad)
freqOptions = [200, 400, 800, 1000]
sf = tk.IntVar(root)
sfMenu = ttk.OptionMenu(root, sf, 200, *freqOptions)
sfMenu.grid(row=0, column=1, sticky=tk.NW, padx=xPad, pady=yPad)
'''

# File Length
flLabel = ttk.Label(root, text='File Length:')
flLabel.grid(row=0, column=0, sticky=tk.NW, padx=xPad, pady=yPad)


def validator(char):  # Validator function so only numbers can be typed in textbox
    return char.isdigit()


flEntry = ttk.Entry(root, validate="key", validatecommand=(root.register(validator), '%S'))
flEntry.grid(row=0, column=3, sticky=tk.NW, padx=xPad, pady=yPad)
flUnits = ['s', 'min', 'hr']
fl = tk.StringVar(root)
flMenu = ttk.OptionMenu(root, fl, 's', *flUnits)
flMenu.grid(row=0, column=4, sticky=tk.NW, padx=xPad, pady=yPad)

# Duty Cycle
dcLabel = ttk.Label(root, text='Duty Cycle?')
dcLabel.grid(row=1, column=0, sticky=tk.NW, padx=xPad, pady=yPad)


def boxChecked():
    vals = [trLabel, trEntry, trMenu, twLabel, twEntry, twMenu]
    for val in vals:
        val.config(state=dc.get())


dc = tk.StringVar(root)
dcBox = ttk.Checkbutton(root, onvalue='enabled', offvalue='disabled', variable=dc, command=boxChecked)
dcBox.grid(row=1, column=1, sticky=tk.NW, padx=xPad, pady=yPad)
trLabel = ttk.Label(root, text='Time Recording:', state='disabled')
trLabel.grid(row=1, column=2, sticky=tk.NW, padx=xPad, pady=yPad)
trEntry = ttk.Entry(root, validate='key', validatecommand=(root.register(validator), '%S'), state='disabled')
trEntry.grid(row=1, column=3, sticky=tk.NW, padx=xPad, pady=yPad)
dcUnits = ['s', 'min', 'hr', 'day']
tr = tk.StringVar(root)
trMenu = ttk.OptionMenu(root, tr, 's', *dcUnits)
trMenu.config(state='disabled')
trMenu.grid(row=1, column=4, sticky=tk.NW, padx=xPad, pady=yPad)
ttk.Label(root, text='').grid(row=2, column=0, sticky=tk.NW, padx=xPad, pady=yPad)
ttk.Label(root, text='').grid(row=2, column=1, sticky=tk.NW, padx=xPad, pady=yPad)
twLabel = ttk.Label(root, text='Time Waiting:', state='disabled')
twLabel.grid(row=2, column=2, sticky=tk.NW, padx=xPad, pady=yPad)
twEntry = ttk.Entry(root, validate='key', validatecommand=(root.register(validator), '%S'), state='disabled')
twEntry.grid(row=2, column=3, sticky=tk.NW, padx=xPad, pady=yPad)
tw = tk.StringVar(root)
twMenu = ttk.OptionMenu(root, tw, 's', *dcUnits)
twMenu.config(state='disabled')
twMenu.grid(row=2, column=4, sticky=tk.NW, padx=xPad, pady=yPad)

# Folder Selection

def selectFolder():  # Creates file selection function
    global directory
    directory = fd.askdirectory()
    fileLabel.config(text=str(directory))


fileButton = ttk.Button(root, text='Select SD Card Folder', command=selectFolder)
fileButton.grid(row=3, column=0, sticky=tk.NW, padx=xPad, pady=yPad)
fileLabel = ttk.Label(root, text='')
fileLabel.grid(row=3, column=1, columnspan=3, sticky=tk.NW, padx=xPad, pady=yPad)


# Saving Settings
def saveSettings():
    units = {'s': 1, 'min': 60, 'hr': 60 * 60, 'day': 60 * 60 * 24}
    try:
        print(directory)
    except:
        tk.messagebox.showerror('No Folder Selected', 'Please select the hydrophone SD card.')
    else:
        if flEntry.get() == '':
            tk.messagebox.showerror('Enter File Length', 'Please enter the maximum file length.')
        if dc.get() == 'enabled' and (trEntry.get() == '' or twEntry.get() == ''):
            tk.messagebox.showerror('Enter Time', 'Please enter both the time waiting and time recording for the duty cycle.')
        else:
            f = open(directory + '/settings.txt', 'w')
            lenFile = int(flEntry.get())*units[fl.get()]
            f.write(str(lenFile) + '\n')
            if dc.get() == 'enabled':
                lenRecord = int(trEntry.get())*units[tr.get()]
                lenWait = int(twEntry.get()) * units[tw.get()]
                f.write('1' + '\n' + str(lenRecord) + '\n' + str(lenWait) + '\n')
            else:
                f.write('0\n')
            f.close()
            return

saveButton = ttk.Button(root, text='Save Settings', command=saveSettings)
saveButton.grid(row=4, column=0, sticky=tk.NW, padx=xPad, pady=yPad)

root.mainloop()
