namespace lab2_comp_graph
{
    partial class Form1
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend3 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.chartRed = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chartGreen = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chartBlue = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.pictureRed = new System.Windows.Forms.PictureBox();
            this.pictureGreen = new System.Windows.Forms.PictureBox();
            this.pictureBlue = new System.Windows.Forms.PictureBox();
            this.pictureOriginal = new System.Windows.Forms.PictureBox();
            this.buttonLoad = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.chartRed)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartGreen)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartBlue)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureRed)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureGreen)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBlue)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureOriginal)).BeginInit();
            this.SuspendLayout();
            // 
            // chartRed
            // 
            chartArea1.Name = "ChartArea1";
            this.chartRed.ChartAreas.Add(chartArea1);
            legend1.Name = "Legend1";
            this.chartRed.Legends.Add(legend1);
            this.chartRed.Location = new System.Drawing.Point(749, 394);
            this.chartRed.Name = "chartRed";
            series1.ChartArea = "ChartArea1";
            series1.Legend = "Legend1";
            series1.Name = "Series1";
            this.chartRed.Series.Add(series1);
            this.chartRed.Size = new System.Drawing.Size(381, 300);
            this.chartRed.TabIndex = 0;
            this.chartRed.Text = "chart1";
            // 
            // chartGreen
            // 
            chartArea2.Name = "ChartArea1";
            this.chartGreen.ChartAreas.Add(chartArea2);
            legend2.Name = "Legend1";
            this.chartGreen.Legends.Add(legend2);
            this.chartGreen.Location = new System.Drawing.Point(1136, 394);
            this.chartGreen.Name = "chartGreen";
            series2.ChartArea = "ChartArea1";
            series2.Legend = "Legend1";
            series2.Name = "Series1";
            this.chartGreen.Series.Add(series2);
            this.chartGreen.Size = new System.Drawing.Size(381, 300);
            this.chartGreen.TabIndex = 1;
            this.chartGreen.Text = "chart2";
            // 
            // chartBlue
            // 
            chartArea3.Name = "ChartArea1";
            this.chartBlue.ChartAreas.Add(chartArea3);
            legend3.Name = "Legend1";
            this.chartBlue.Legends.Add(legend3);
            this.chartBlue.Location = new System.Drawing.Point(1540, 394);
            this.chartBlue.Name = "chartBlue";
            series3.ChartArea = "ChartArea1";
            series3.Legend = "Legend1";
            series3.Name = "Series1";
            this.chartBlue.Series.Add(series3);
            this.chartBlue.Size = new System.Drawing.Size(381, 300);
            this.chartBlue.TabIndex = 2;
            this.chartBlue.Text = "chart3";
            // 
            // pictureRed
            // 
            this.pictureRed.Location = new System.Drawing.Point(749, 61);
            this.pictureRed.Name = "pictureRed";
            this.pictureRed.Size = new System.Drawing.Size(381, 269);
            this.pictureRed.TabIndex = 3;
            this.pictureRed.TabStop = false;
            // 
            // pictureGreen
            // 
            this.pictureGreen.Location = new System.Drawing.Point(1136, 61);
            this.pictureGreen.Name = "pictureGreen";
            this.pictureGreen.Size = new System.Drawing.Size(381, 269);
            this.pictureGreen.TabIndex = 4;
            this.pictureGreen.TabStop = false;
            // 
            // pictureBlue
            // 
            this.pictureBlue.Location = new System.Drawing.Point(1540, 61);
            this.pictureBlue.Name = "pictureBlue";
            this.pictureBlue.Size = new System.Drawing.Size(381, 269);
            this.pictureBlue.TabIndex = 5;
            this.pictureBlue.TabStop = false;
            // 
            // pictureOriginal
            // 
            this.pictureOriginal.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureOriginal.Location = new System.Drawing.Point(59, 61);
            this.pictureOriginal.Name = "pictureOriginal";
            this.pictureOriginal.Size = new System.Drawing.Size(644, 633);
            this.pictureOriginal.TabIndex = 6;
            this.pictureOriginal.TabStop = false;
            // 
            // buttonLoad
            // 
            this.buttonLoad.Location = new System.Drawing.Point(292, 741);
            this.buttonLoad.Name = "buttonLoad";
            this.buttonLoad.Size = new System.Drawing.Size(194, 58);
            this.buttonLoad.TabIndex = 7;
            this.buttonLoad.Text = "Load Image";
            this.buttonLoad.UseVisualStyleBackColor = true;
            this.buttonLoad.Click += new System.EventHandler(this.button1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(12F, 25F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1947, 875);
            this.Controls.Add(this.buttonLoad);
            this.Controls.Add(this.pictureOriginal);
            this.Controls.Add(this.pictureBlue);
            this.Controls.Add(this.pictureGreen);
            this.Controls.Add(this.pictureRed);
            this.Controls.Add(this.chartBlue);
            this.Controls.Add(this.chartGreen);
            this.Controls.Add(this.chartRed);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.chartRed)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartGreen)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartBlue)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureRed)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureGreen)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBlue)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureOriginal)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataVisualization.Charting.Chart chartRed;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartGreen;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartBlue;
        private System.Windows.Forms.PictureBox pictureRed;
        private System.Windows.Forms.PictureBox pictureGreen;
        private System.Windows.Forms.PictureBox pictureBlue;
        private System.Windows.Forms.PictureBox pictureOriginal;
        private System.Windows.Forms.Button buttonLoad;
    }
}

