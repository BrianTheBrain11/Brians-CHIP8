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
            SuspendLayout();
            // 
            // ProgramTextBox
            // 
            ProgramTextBox.AccessibleDescription = "Space for inputting Chip8 program.";
            ProgramTextBox.AccessibleName = "Program Text Box";
            ProgramTextBox.AccessibleRole = AccessibleRole.Document;
            ProgramTextBox.Location = new Point(22, 65);
            ProgramTextBox.Multiline = true;
            ProgramTextBox.Name = "ProgramTextBox";
            ProgramTextBox.Size = new Size(979, 571);
            ProgramTextBox.TabIndex = 0;
            ProgramTextBox.TextChanged += ProgramTextBox_TextChanged;
            // 
            // AssembleButton
            // 
            AssembleButton.AccessibleDescription = "Button to being assembly";
            AssembleButton.AccessibleName = "AssembleButton";
            AssembleButton.AccessibleRole = AccessibleRole.PushButton;
            AssembleButton.Location = new Point(22, 16);
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
            OutputFileBrowseButton.Location = new Point(127, 16);
            OutputFileBrowseButton.Name = "OutputFileBrowseButton";
            OutputFileBrowseButton.RightToLeft = RightToLeft.Yes;
            OutputFileBrowseButton.Size = new Size(874, 37);
            OutputFileBrowseButton.TabIndex = 2;
            OutputFileBrowseButton.TextAlign = ContentAlignment.MiddleRight;
            OutputFileBrowseButton.UseVisualStyleBackColor = true;
            OutputFileBrowseButton.Click += OutputFileBrowseButton_Click;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1027, 648);
            Controls.Add(OutputFileBrowseButton);
            Controls.Add(AssembleButton);
            Controls.Add(ProgramTextBox);
            Name = "Form1";
            Text = "Form1";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private TextBox ProgramTextBox;
        private Button AssembleButton;
        private SaveFileDialog OutputFileDialog;
        private Button OutputFileBrowseButton;
    }
}
