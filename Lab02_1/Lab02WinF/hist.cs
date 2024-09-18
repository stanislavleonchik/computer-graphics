using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace Lab02WinF {
    public partial class hist : Form {
        private List<int> histd = new List<int>(256);
        int maxValue;
        public hist(int[] points) {
            InitializeComponent();
            this.histd = new List<int>(points);
            maxValue = histd.Max();

            DrawHistogram();
        }

        private void DrawHistogram() {
            // Создаем объект для рисования
            Bitmap bitmap = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            using (Graphics g = Graphics.FromImage(bitmap)) {
                g.Clear(Color.White);  // Очищаем фон

                // Настройки для рисования гистограммы
                int barWidth = pictureBox1.Width / histd.Count;  // Ширина одного столбца
               
                for (int i = 0; i < histd.Count; i++) {
                    int barHeight = (int)((histd[i] / (float)maxValue) * pictureBox1.Height);
                    // Рисуем столбец
                    g.FillRectangle(Brushes.Blue, i * barWidth, pictureBox1.Height - barHeight, barWidth, barHeight);
                    g.DrawRectangle(Pens.Black, i * barWidth, pictureBox1.Height - barHeight, barWidth, barHeight);
                }
            }

            pictureBox1.Image = bitmap;
        }

        private void pictureBox1_Click(object sender, EventArgs e) {

        }
    }
}
