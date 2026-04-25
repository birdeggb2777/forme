using System;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace onlytext
{
    public partial class Form1 : Form
    {
        string filePath = "";
        public Form1(){ InitializeComponent(); }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.AllowDrop = true;
            this.DragEnter += new DragEventHandler(dragEnter);
            this.DragDrop += new DragEventHandler(dragDrop);

            this.WindowState = FormWindowState.Maximized;
            string[] args = Environment.GetCommandLineArgs();
            if (args.Length == 2)
            {
                try
                {
                    string fileContent = System.IO.File.ReadAllText(args[1], System.Text.Encoding.UTF8);
                    textBox.Text = fileContent;
                    this.Text = filePath = args[1];
                }
                catch (Exception ex)
                {
                    MessageBox.Show("檔案讀取失敗，原因窩也不知道(╥﹏╥)");
                }
            }
        }

        private void 開啟舊檔ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openFileDialog.Title = "選擇文字檔案";
            openFileDialog.Filter = "文字檔 (*.txt)|*.txt|文字檔 (*.*)|*.*";
            openFileDialog.FilterIndex = 1;
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    string fileContent = System.IO.File.ReadAllText(openFileDialog.FileName, System.Text.Encoding.UTF8);
                    textBox.Text = fileContent;
                    this.Text = filePath = openFileDialog.FileName;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("檔案讀取失敗，原因窩也不知道(╥﹏╥)");
                }
            }
            else MessageBox.Show("檔案讀取失敗，原因窩也不知道(╥﹏╥)");
        }

        private void 儲存ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveFileDialog.Filter = "文字檔 (*.txt)|*.txt|文字檔 (*.*)|*.*";
            saveFileDialog.Title = "儲存檔案";
            saveFileDialog.DefaultExt = "txt";

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    File.WriteAllText(saveFileDialog.FileName, "" + textBox.Text);
                    filePath = saveFileDialog.FileName;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("檔案讀取失敗，原因窩也不知道(╥﹏╥)");
                }
            }
        }
        private async void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            if (filePath != "" && e.Control && e.KeyCode == Keys.S)
            {
                File.WriteAllText(filePath, "" + textBox.Text);
                this.Text = "儲存完畢";
                await Task.Delay(1000);
                this.Text = filePath;
            }
        }
        private void dragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop)) e.Effect = DragDropEffects.Copy;
            else e.Effect = DragDropEffects.None;
        }
        private void dragDrop(object sender, DragEventArgs e)
        {
            if (filePath != "") return;
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            if (files != null && files.Length == 1)
            {
                string fileContent = System.IO.File.ReadAllText(files[0], System.Text.Encoding.UTF8);
                textBox.Text = fileContent;
                this.Text = filePath = files[0];
            }
        }
    }
}
