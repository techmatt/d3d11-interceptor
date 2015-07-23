namespace UIWindow
{
    partial class UIWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.timerProcessMessages = new System.Windows.Forms.Timer(this.components);
            this.timerInitialize = new System.Windows.Forms.Timer(this.components);
            this.timerDisconnectCheck = new System.Windows.Forms.Timer(this.components);
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // timerProcessMessages
            // 
            this.timerProcessMessages.Enabled = true;
            this.timerProcessMessages.Interval = 1;
            this.timerProcessMessages.Tick += new System.EventHandler(this.timerProcessMessages_Tick);
            // 
            // timerInitialize
            // 
            this.timerInitialize.Enabled = true;
            this.timerInitialize.Tick += new System.EventHandler(this.timerInitialize_Tick);
            // 
            // timerDisconnectCheck
            // 
            this.timerDisconnectCheck.Enabled = true;
            this.timerDisconnectCheck.Interval = 500;
            this.timerDisconnectCheck.Tick += new System.EventHandler(this.timerDisconnectCheck_Tick);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(12, 12);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(108, 27);
            this.button1.TabIndex = 0;
            this.button1.Text = "Load frame";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // UIWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(408, 382);
            this.Controls.Add(this.button1);
            this.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "UIWindow";
            this.Opacity = 0.8D;
            this.Text = "Waiting for connection from application...";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UIWindow_FormClosing);
            this.Load += new System.EventHandler(this.UIWindow_Load);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.UIWindow_KeyUp);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Timer timerProcessMessages;
        private System.Windows.Forms.Timer timerInitialize;
        private System.Windows.Forms.Timer timerDisconnectCheck;
        private System.Windows.Forms.Button button1;
    }
}

