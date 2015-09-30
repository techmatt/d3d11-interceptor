﻿namespace UIWindow
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
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxFrameIndex = new System.Windows.Forms.TextBox();
            this.buttonLoadFrame = new System.Windows.Forms.Button();
            this.buttonTestAnchor = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxReplayIndex = new System.Windows.Forms.TextBox();
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
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(32, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(44, 15);
            this.label2.TabIndex = 1;
            this.label2.Text = "Frame:";
            // 
            // textBoxFrameIndex
            // 
            this.textBoxFrameIndex.Location = new System.Drawing.Point(82, 48);
            this.textBoxFrameIndex.Name = "textBoxFrameIndex";
            this.textBoxFrameIndex.Size = new System.Drawing.Size(219, 23);
            this.textBoxFrameIndex.TabIndex = 2;
            this.textBoxFrameIndex.Text = "0";
            // 
            // buttonLoadFrame
            // 
            this.buttonLoadFrame.Location = new System.Drawing.Point(313, 29);
            this.buttonLoadFrame.Name = "buttonLoadFrame";
            this.buttonLoadFrame.Size = new System.Drawing.Size(82, 27);
            this.buttonLoadFrame.TabIndex = 0;
            this.buttonLoadFrame.Text = "Load";
            this.buttonLoadFrame.UseVisualStyleBackColor = true;
            this.buttonLoadFrame.Click += new System.EventHandler(this.buttonLoadFrame_Click);
            // 
            // buttonTestAnchor
            // 
            this.buttonTestAnchor.Location = new System.Drawing.Point(11, 88);
            this.buttonTestAnchor.Name = "buttonTestAnchor";
            this.buttonTestAnchor.Size = new System.Drawing.Size(91, 27);
            this.buttonTestAnchor.TabIndex = 0;
            this.buttonTestAnchor.Text = "Test anchor";
            this.buttonTestAnchor.UseVisualStyleBackColor = true;
            this.buttonTestAnchor.Click += new System.EventHandler(this.buttonTestAnchor_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(32, 20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 15);
            this.label3.TabIndex = 1;
            this.label3.Text = "Replay:";
            // 
            // textBoxReplayIndex
            // 
            this.textBoxReplayIndex.Location = new System.Drawing.Point(82, 17);
            this.textBoxReplayIndex.Name = "textBoxReplayIndex";
            this.textBoxReplayIndex.Size = new System.Drawing.Size(219, 23);
            this.textBoxReplayIndex.TabIndex = 2;
            this.textBoxReplayIndex.Text = "0";
            // 
            // UIWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(408, 382);
            this.Controls.Add(this.textBoxReplayIndex);
            this.Controls.Add(this.textBoxFrameIndex);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.buttonTestAnchor);
            this.Controls.Add(this.buttonLoadFrame);
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
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer timerProcessMessages;
        private System.Windows.Forms.Timer timerInitialize;
        private System.Windows.Forms.Timer timerDisconnectCheck;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxFrameIndex;
        private System.Windows.Forms.Button buttonLoadFrame;
        private System.Windows.Forms.Button buttonTestAnchor;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBoxReplayIndex;
    }
}
