using System.Runtime.InteropServices;

namespace Chip8_Assembler
{
    public partial class Form1 : Form
    {
        Dictionary<string, Func<object[], UInt16>> operations = new Dictionary<string, Func<object[], UInt16>>();

        public Form1()
        {
            InitializeComponent();

            ConsoleAllocator.ShowConsoleWindow();


            operations["LD"] = (args) =>
            {
                if (args.Length != 3)
                {
                    throw new ArgumentException("Wrong number of arguments for LD");
                }

                char firstCharOfArg1 = ((string)args[1])[0];
                if (firstCharOfArg1 != 'V' && firstCharOfArg1 != 'I')
                {
                    throw new ArgumentException("First argument is not a V register or I");
                }

                if (firstCharOfArg1 == 'I')
                {
                    byte opcode = (byte)0xA;
                    UInt16 value = (UInt16)int.Parse((string)args[2]);
                    byte sourceRegister = (byte)int.Parse(((string)args[2]).AsSpan(1));
                    UInt16 op = (UInt16)
                    (
                        (value) | // bits 0-11
                        (opcode << 12) // bits 12-15
                    );
                    return op;
                }

                byte targetRegister = (byte)int.Parse(((string)args[1]).AsSpan(1));

                char firstCharOfArg2 = ((string)args[2])[0];
                if (firstCharOfArg2 == 'V')
                {
                    byte opcode = (byte)0x8;
                    byte sourceRegister = (byte)int.Parse(((string)args[2]).AsSpan(1));
                    UInt16 op = (UInt16)
                    (
                        ((byte)0x00) | // bits 0-3
                        (sourceRegister << 4) | // bits 3-7
                        (targetRegister << 8) | // bits 8-11
                        (opcode << 12) // bits 12-15
                    );
                    return op;
                }
                else
                {

                    byte opcode = (byte)0x6;
                    byte value = (byte)int.Parse((string)args[2]);
                    UInt16 op = (UInt16)
                    (
                        (value) | // bits 0-7
                        (targetRegister << 8) | // bits 8-11
                        (opcode << 12) // bits 12-15
                    );
                    return op;
                }
            };
            operations["CLS"] = (args) =>
            {
                return (UInt16)0x00E0;
            };
            operations["DRW"] = (args) =>
            {
                Console.WriteLine("In a draw op");
                if (args.Length != 4)
                {
                    throw new ArgumentException("Wrong number of arguments for DRW");
                }
                char firstCharOfArg1 = ((string)args[1])[0];
                if (firstCharOfArg1 != 'V')
                {
                    throw new ArgumentException("First argument is not a V register");
                }
                char firstCharOfArg2 = ((string)args[2])[0];
                if (firstCharOfArg2 != 'V')
                {
                    throw new ArgumentException("First argument is not a V register");
                }

                byte opcode = 0xD;

                byte argumentRegister1 = (byte)int.Parse(((string)args[1]).AsSpan(1));
                byte argumentRegister2 = (byte)int.Parse(((string)args[2]).AsSpan(1));

                byte drawBytesCount = (byte)int.Parse((string)args[3]);

                UInt16 op = (UInt16)
                (
                    (drawBytesCount) | // bits 0-3
                    (argumentRegister2 << 4) | // bits 3-7
                    (argumentRegister1 << 8) | // bits 8-11
                    (opcode << 12) // bits 12-15
                );

                return op;
            };
        }

        private void ProgramTextBox_TextChanged(object sender, EventArgs e)
        {

        }

        private void AssembleButton_Click(object sender, EventArgs e)
        {
            string path = OutputFileDialog.FileName;

            List<byte> bytesToWrite = new List<byte>();

            foreach (string line in ProgramTextBox.Lines)
            {
                string[] words = line.Split(' ');
                UInt16 op = (UInt16)operations[words[0]](words);

                byte highByte = (byte)(op >> 8); // upper byte
                byte lowByte = (byte)(op & 0x00FF); // lower byte

                // write high byte first because we are big endian
                bytesToWrite.Add(highByte);
                bytesToWrite.Add(lowByte);
            }

            File.WriteAllBytes(path, bytesToWrite.ToArray());
        }

        private void OutputFileBrowseButton_Click(object sender, EventArgs e)
        {
            if (OutputFileDialog.ShowDialog() == DialogResult.OK)
            {
                OutputFileBrowseButton.Text = OutputFileDialog.FileName;
            }
        }

        private void SaveToFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                File.WriteAllLines(saveFileDialog.FileName, ProgramTextBox.Lines);
            }
        }

        private void LoadFromFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                ProgramTextBox.Lines = File.ReadAllLines(openFileDialog.FileName);
            }
        }

        private void openFileDialog_FileOk(object sender, System.ComponentModel.CancelEventArgs e)
        {

        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }
    }

    internal static class ConsoleAllocator
    {
        [DllImport(@"kernel32.dll", SetLastError = true)]
        static extern bool AllocConsole();

        [DllImport(@"kernel32.dll")]
        static extern IntPtr GetConsoleWindow();

        [DllImport(@"user32.dll")]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        const int SwHide = 0;
        const int SwShow = 5;


        public static void ShowConsoleWindow()
        {
            var handle = GetConsoleWindow();

            if (handle == IntPtr.Zero)
            {
                AllocConsole();
            }
            else
            {
                ShowWindow(handle, SwShow);
            }
        }

        public static void HideConsoleWindow()
        {
            var handle = GetConsoleWindow();

            ShowWindow(handle, SwHide);
        }
    }
}
