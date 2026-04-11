using System.Runtime.InteropServices;
using System.Text;

namespace translate
{
    public partial class TranslateWindow : Form
    {
        // --- 引入 Windows API ---
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool AddClipboardFormatListener(IntPtr hwnd);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool RemoveClipboardFormatListener(IntPtr hwnd);
        // 定義剪貼簿更新的系統訊息代碼
        private const int WM_CLIPBOARDUPDATE = 0x031D;

        private Dictionary<string, string> En2ZhDict;
        public TranslateWindow() {InitializeComponent();}
        private bool isDictReady = false;

        private void LoadCSV()
        {
            // ecdict_tw_ctt.csv:(word, translation, phonetic)
            foreach (var line in File.ReadLines("ecdict_tw_ctt.csv", Encoding.Default))
            {
                var parts = line.Split(',');
                if (parts.Length < 2) continue; // 略過格式不符的行數
                // 英翻中
                var key = parts[0]; var value = parts[1].Replace("\\n", "、");
                if (En2ZhDict.TryGetValue(key, out string existingValue))
                    En2ZhDict[key] = existingValue + "、" + value; // 如果 Key 已經存在，就加上 \n 與新內容
                else
                    En2ZhDict[key] = value; // 如果是新的 Key，直接加入
            }
        }

        private async void TranslateWindow_Load(object sender, EventArgs e)
        {
            if (!File.Exists("ecdict_tw_ctt.csv")) targetLabel.Text = "找不到「ecdict_tw_ctt.csv」這個字典檔案\n程式無法正常運作(╥﹏╥)";
            else
            {
                this.Text = "字典檔載入中，請稍後...";
                En2ZhDict = new Dictionary<string, string>();
                AddClipboardFormatListener(this.Handle);
                this.TopMost = true; // 視窗永遠最上層
                targetLabel.Text = "歡迎使用本人專用的翻譯工具，字典檔改編自skywind3000/ECDICT。\n隨意複製一個單字，就能立即顯示翻譯結果了，祝您學習愉快ξ( ✿＞◡❛)";
                await Task.Run(() => { LoadCSV();}); // 在背景載入
                isDictReady = true;
                this.Text = "翻譯工具";
            }
        }
        protected override void WndProc(ref Message m)
        {
            // 如果接收到「剪貼簿更新」的訊息
            if (m.Msg == WM_CLIPBOARDUPDATE && isDictReady) OnClipboardChanged();
            base.WndProc(ref m); // 讓系統繼續處理其他的視窗訊息
        }
        private async void OnClipboardChanged()
        {
            await Task.Delay(200);
            try
            {
                if (Clipboard.ContainsText())
                {
                    string selectedText = Clipboard.GetText().Trim();
                    if (En2ZhDict.TryGetValue(selectedText.ToLower(), out string meaning))
                        targetLabel.Text = $"英文：{selectedText}\n翻譯：{meaning}";
                }
            }
            catch (Exception ex)
            { Console.WriteLine("讀取剪貼簿失敗: " + ex.Message); }
        }
        public void Form1_FormClosing(object sender, FormClosingEventArgs e)
        { RemoveClipboardFormatListener(this.Handle); } // 程式關閉時，向作業系統解除註冊
    }
}
