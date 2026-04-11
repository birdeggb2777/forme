namespace translate
{
    partial class TranslateWindow
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
            targetLabel = new Label();
            SuspendLayout();
            // 
            // targetLabel
            // 
            targetLabel.AutoSize = true;
            targetLabel.Font = new Font("微軟正黑體", 15.8571434F, FontStyle.Bold, GraphicsUnit.Point, 136);
            targetLabel.Location = new Point(12, 25);
            targetLabel.Name = "targetLabel";
            targetLabel.Size = new Size(287, 94);
            targetLabel.TabIndex = 0;
            targetLabel.Text = "英文：translate\r\n中文：翻譯\r\n";
            // 
            // TranslateWindow
            // 
            AutoScaleDimensions = new SizeF(12F, 26F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1567, 150);
            Controls.Add(targetLabel);
            Name = "TranslateWindow";
            Text = "翻譯";
            Load += TranslateWindow_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label targetLabel;
    }
}
