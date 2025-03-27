namespace Chip8_Assembler
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            ProgramTextBox = new TextBox();
            AssembleButton = new Button();
            OutputFileDialog = new SaveFileDialog();
            OutputFileBrowseButton = new Button();
            menuStrip = new MenuStrip();
            fileToolStripMenuItem = new ToolStripMenuItem();
            saveToFileToolStripMenuItem = new ToolStripMenuItem();
            loadFromFileToolStripMenuItem = new ToolStripMenuItem();
            saveFileDialog = new SaveFileDialog();
            openFileDialog = new OpenFileDialog();
            menuStrip.SuspendLayout();
            SuspendLayout();
            // 
            // ProgramTextBox
            // 
            ProgramTextBox.AccessibleDescription = "Space for inputting Chip8 program.";
            ProgramTextBox.AccessibleName = "Program Text Box";
            ProgramTextBox.AccessibleRole = AccessibleRole.Document;
            ProgramTextBox.Location = new Point(22, 82);
            ProgramTextBox.Multiline = true;
            ProgramTextBox.Name = "ProgramTextBox";
            ProgramTextBox.Size = new Size(979, 554);
            ProgramTextBox.TabIndex = 0;
            ProgramTextBox.TextChanged += ProgramTextBox_TextChanged;
            // 
            // AssembleButton
            // 
            AssembleButton.AccessibleDescription = "Button to being assembly";
            AssembleButton.AccessibleName = "AssembleButton";
            AssembleButton.AccessibleRole = AccessibleRole.PushButton;
            AssembleButton.Location = new Point(22, 27);
            AssembleButton.Name = "AssembleButton";
            AssembleButton.Size = new Size(94, 37);
            AssembleButton.TabIndex = 1;
            AssembleButton.Text = "Aseemble";
            AssembleButton.UseVisualStyleBackColor = true;
            AssembleButton.Click += AssembleButton_Click;
            // 
            // OutputFileDialog
            // 
            OutputFileDialog.Filter = "Chip8 Files (*.ch8)|*.ch8|All Files (*.*)|*.*";
            // 
            // OutputFileBrowseButton
            // 
            OutputFileBrowseButton.AccessibleDescription = "Button to select the file to which the program will be saved to.";
            OutputFileBrowseButton.AccessibleName = "Output file browse";
            OutputFileBrowseButton.Location = new Point(122, 27);
            OutputFileBrowseButton.Name = "OutputFileBrowseButton";
            OutputFileBrowseButton.RightToLeft = RightToLeft.Yes;
            OutputFileBrowseButton.Size = new Size(879, 37);
            OutputFileBrowseButton.TabIndex = 2;
            OutputFileBrowseButton.TextAlign = ContentAlignment.MiddleRight;
            OutputFileBrowseButton.UseVisualStyleBackColor = true;
            OutputFileBrowseButton.Click += OutputFileBrowseButton_Click;
            // 
            // menuStrip
            // 
            menuStrip.Items.AddRange(new ToolStripItem[] { fileToolStripMenuItem });
            menuStrip.Location = new Point(0, 0);
            menuStrip.Name = "menuStrip";
            menuStrip.Size = new Size(1027, 24);
            menuStrip.TabIndex = 3;
            menuStrip.Text = "menuStrip1";
            menuStrip.ItemClicked += menuStrip1_ItemClicked;
            // 
            // fileToolStripMenuItem
            // 
            fileToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { saveToFileToolStripMenuItem, loadFromFileToolStripMenuItem });
            fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            fileToolStripMenuItem.Size = new Size(37, 20);
            fileToolStripMenuItem.Text = "File";
            // 
            // saveToFileToolStripMenuItem
            // 
            saveToFileToolStripMenuItem.AccessibleDescription = "Save whats currently in the program text box to a file.";
            saveToFileToolStripMenuItem.AccessibleName = "Save to file";
            saveToFileToolStripMenuItem.AccessibleRole = AccessibleRole.MenuItem;
            saveToFileToolStripMenuItem.Name = "saveToFileToolStripMenuItem";
            saveToFileToolStripMenuItem.Size = new Size(148, 22);
            saveToFileToolStripMenuItem.Text = "Save to file";
            saveToFileToolStripMenuItem.Click += SaveToFileToolStripMenuItem_Click;
            // 
            // loadFromFileToolStripMenuItem
            // 
            loadFromFileToolStripMenuItem.AccessibleDescription = "Loads an assembly file and puts the contents in the program text box.";
            loadFromFileToolStripMenuItem.AccessibleName = "Load from file";
            loadFromFileToolStripMenuItem.Name = "loadFromFileToolStripMenuItem";
            loadFromFileToolStripMenuItem.Size = new Size(148, 22);
            loadFromFileToolStripMenuItem.Text = "Load from file";
            loadFromFileToolStripMenuItem.Click += LoadFromFileToolStripMenuItem_Click;
            // 
            // saveFileDialog
            // 
            saveFileDialog.Filter = "Chip8 Assembly file (*.s)|*.s|All Files (*.*)|*.*";
            // 
            // openFileDialog
            // 
            openFileDialog.Filter = "Chip8 Assembly file (*.s)|*.s|All Files (*.*)|*.*";
            openFileDialog.FileOk += openFileDialog_FileOk;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1027, 648);
            Controls.Add(OutputFileBrowseButton);
            Controls.Add(AssembleButton);
            Controls.Add(ProgramTextBox);
            Controls.Add(menuStrip);
            MainMenuStrip = menuStrip;
            Name = "Form1";
            Text = "Form1";
            menuStrip.ResumeLayout(false);
            menuStrip.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private TextBox ProgramTextBox;
        private Button AssembleButton;
        private SaveFileDialog OutputFileDialog;
        private Button OutputFileBrowseButton;
        private MenuStrip menuStrip;
        private ToolStripMenuItem fileToolStripMenuItem;
        private ToolStripMenuItem saveToFileToolStripMenuItem;
        private ToolStripMenuItem loadFromFileToolStripMenuItem;
        private SaveFileDialog saveFileDialog;
        private OpenFileDialog openFileDialog;
    }
}
