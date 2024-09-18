using System.Drawing;
using FastBitmap;

namespace Lab02WinF {
    public partial class Form1 : Form {
        private Bitmap bmp;
        private Bitmap g1_bmp;
        private Bitmap g2_bmp;
        private Bitmap diff_bmp;
        private int[] points1;
        private int[] points2;
        private int[] pointsd;
        public Form1() {
            InitializeComponent();

            bmp = new Bitmap("C:\\Users\\mocal\\OneDrive\\Изображения\\слайд-шоу\\00_2561_Baggerschiff_Nexus_(2014)_Van_Oord.jpg");
            pictureBox1.Image = bmp;
            pictureBox1.Invalidate();

            formula1();
            formula2();
            diff();
        }

        void formula1() {
            points1 = new int[256];
            using (var fastBitmap = new FastBitmap.FastBitmap(bmp)) {
                g1_bmp = fastBitmap.Select(color => {
                    int Y = (int)Math.Round(0.3 * color.R + 0.59 * color.G + 0.11 * color.B);
                    points1[Y]++;
                    return Color.FromArgb(Y, Y, Y);
                });
            }
        }
         
        void formula2() {
            points2 = new int[256];
            using (var fastBitmap = new FastBitmap.FastBitmap(bmp)) {
                g2_bmp = fastBitmap.Select(color => {
                    int Y = (int)Math.Round(0.21 * color.R + 0.72 * color.G + 0.07 * color.B);
                    points2[Y]++;
                    return Color.FromArgb(Y, Y, Y);
                });
            }
        }
        void diff() {
            diff_bmp = new Bitmap(pictureBox1.Image);
            pointsd = new int[256];
            using (var fastBitmap = new FastBitmap.FastBitmap(diff_bmp)) {
                var fastBitmap1 = new FastBitmap.FastBitmap(g1_bmp);
                var fastBitmap2 = new FastBitmap.FastBitmap(g2_bmp);
                for (var x = 0; x < fastBitmap.Width; x++)
                    for (var y = 0; y < fastBitmap.Height; y++) {
                        var color1 = fastBitmap1[x, y];
                        var color2 = fastBitmap2[x, y];
                        int d = Math.Abs((color2.R - color1.R) % 255);
                        pointsd[d]++;
                        fastBitmap[x, y] = Color.FromArgb(255 - d, 255 - d, 255 - d);
                    }
                fastBitmap1.Dispose();
                fastBitmap2.Dispose();
            }
        }

        private void pictureBox1_Click(object sender, EventArgs e) {
        }

        private void toolStripButton2_Click(object sender, EventArgs e) {
            pictureBox1.Image = g1_bmp;
            pictureBox1.Invalidate();
            hist h = new hist(points1);
            h.Show();
        }

        private void toolStripButton1_Click(object sender, EventArgs e) {
            pictureBox1.Image = bmp;
            pictureBox1.Invalidate();
        }

        private void toolStripButton3_Click(object sender, EventArgs e) {
            pictureBox1.Image = g2_bmp;
            pictureBox1.Invalidate();
            hist h = new hist(points2);
            h.Show();
        }

        private void toolStripButton4_Click(object sender, EventArgs e) {
            pictureBox1.Image = diff_bmp;
            pictureBox1.Invalidate();
            hist h = new hist(pointsd);
            h.Show();
        }

        private void toolStripButton5_Click(object sender, EventArgs e) {

        }
    }
}
