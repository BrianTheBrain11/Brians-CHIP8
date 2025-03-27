namespace Chip8_Assembler
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void ProgramTextBox_TextChanged(object sender, EventArgs e)
        {

        }

        private void AssembleButton_Click(object sender, EventArgs e)
        {
            foreach (string line in ProgramTextBox.Lines)
            {

            }
        }

        private void OutputFileBrowseButton_Click(object sender, EventArgs e)
        {
            if (OutputFileDialog.ShowDialog() == DialogResult.OK)
            {
                OutputFileBrowseButton.Text = OutputFileDialog.FileName;
            }
        }
    }
}
