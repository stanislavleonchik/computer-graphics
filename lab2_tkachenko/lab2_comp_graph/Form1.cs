using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace lab2_comp_graph
{
    public partial class Form1 : Form
    {
        private Bitmap originalImage, redChannel, greenChannel, blueChannel;

        public Form1()
        {
            InitializeComponent();
            SetupCharts();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // Создаем диалоговое окно для выбора файла
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Image Files|*.jpg;*.jpeg;*.png;*.bmp|All Files|*.*"; // Указываем фильтр для изображений

            if (openFileDialog.ShowDialog() == DialogResult.OK) // Открываем диалоговое окно
            {
                // Загружаем выбранное изображение в pictureBox
                pictureOriginal.Image = System.Drawing.Image.FromFile(openFileDialog.FileName);

                // Изменяем размер pictureBox, чтобы подогнать под размер изображения (необязательно)
                pictureOriginal.SizeMode = PictureBoxSizeMode.Zoom;
            }
            Task2_Load();

        }
        private void SetupCharts()
        {
            chartRed.Series[0] = (new Series { Color = Color.Red, ChartType = SeriesChartType.Column, Name = "R" });
            chartGreen.Series[0] = new Series { Color = Color.Green, ChartType = SeriesChartType.Column, Name = "G" };
            chartBlue.Series[0] = (new Series { Color = Color.Blue, ChartType = SeriesChartType.Column, Name = "B" });
        }

        private void Task2_Load()
        {
            originalImage = (Bitmap)pictureOriginal.Image;
            // изображения для каждого канала 
            redChannel = new Bitmap(originalImage.Width, originalImage.Height);
            greenChannel = new Bitmap(originalImage.Width, originalImage.Height);
            blueChannel = new Bitmap(originalImage.Width, originalImage.Height);

            ProcessImage(); // обработка изображения 
        }


        private void ProcessImage()
        {
            // Каналы
            int[] redHistogram = new int[256];
            int[] greenHistogram = new int[256];
            int[] blueHistogram = new int[256];

            // Обработка изображения и построение гистограмм
            for (int y = 0; y < originalImage.Height; y++)
            {
                for (int x = 0; x < originalImage.Width; x++)
                {
                    Color pixelColor = originalImage.GetPixel(x, y);

                    // установка цветного компонента
                    redChannel.SetPixel(x, y, Color.FromArgb(pixelColor.R, 0, 0));
                    greenChannel.SetPixel(x, y, Color.FromArgb(0, pixelColor.G, 0));
                    blueChannel.SetPixel(x, y, Color.FromArgb(0, 0, pixelColor.B));

                    redHistogram[pixelColor.R]++;
                    greenHistogram[pixelColor.G]++;
                    blueHistogram[pixelColor.B]++;
                }
            }

            // Заполнение гистограмм
            UpdateHistogramChart(chartRed, redHistogram);
            UpdateHistogramChart(chartGreen, greenHistogram);
            UpdateHistogramChart(chartBlue, blueHistogram);

            // Отображаем полученные каналы
            pictureRed.Image = redChannel;
            pictureGreen.Image = greenChannel;
            pictureBlue.Image = blueChannel;
            pictureRed.SizeMode = PictureBoxSizeMode.Zoom;
            pictureGreen.SizeMode = PictureBoxSizeMode.Zoom;
            pictureBlue.SizeMode = PictureBoxSizeMode.Zoom;
        }

        private void UpdateHistogramChart(Chart chart, int[] histogram)
        {
            chart.Series[0].Points.Clear();

            for (int i = 0; i < histogram.Length; i++)
            {
                chart.Series[0].Points.AddXY(i, histogram[i]);
            }

            chart.Invalidate(); // Обновление графика
        }


    }
}
