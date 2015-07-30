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
            this.buttonLoadSignature = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxSignature = new System.Windows.Forms.TextBox();
            this.checkBoxShowBoxes = new System.Windows.Forms.CheckBox();
            this.checkBoxShowFullMesh = new System.Windows.Forms.CheckBox();
            this.checkBoxSelectionOnly = new System.Windows.Forms.CheckBox();
            this.checkBoxTrackable = new System.Windows.Forms.CheckBox();
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
            // buttonLoadSignature
            // 
            this.buttonLoadSignature.Location = new System.Drawing.Point(314, 12);
            this.buttonLoadSignature.Name = "buttonLoadSignature";
            this.buttonLoadSignature.Size = new System.Drawing.Size(82, 27);
            this.buttonLoadSignature.TabIndex = 0;
            this.buttonLoadSignature.Text = "Load";
            this.buttonLoadSignature.UseVisualStyleBackColor = true;
            this.buttonLoadSignature.Click += new System.EventHandler(this.buttonLoadSignature_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(62, 15);
            this.label1.TabIndex = 1;
            this.label1.Text = "Signature:";
            // 
            // textBoxSignature
            // 
            this.textBoxSignature.Location = new System.Drawing.Point(83, 15);
            this.textBoxSignature.Name = "textBoxSignature";
            this.textBoxSignature.Size = new System.Drawing.Size(219, 23);
            this.textBoxSignature.TabIndex = 2;
            // 
            // checkBoxShowBoxes
            // 
            this.checkBoxShowBoxes.AutoSize = true;
            this.checkBoxShowBoxes.Location = new System.Drawing.Point(12, 44);
            this.checkBoxShowBoxes.Name = "checkBoxShowBoxes";
            this.checkBoxShowBoxes.Size = new System.Drawing.Size(58, 19);
            this.checkBoxShowBoxes.TabIndex = 3;
            this.checkBoxShowBoxes.Text = "Boxes";
            this.checkBoxShowBoxes.UseVisualStyleBackColor = true;
            this.checkBoxShowBoxes.CheckedChanged += new System.EventHandler(this.checkBoxShowBoxes_CheckedChanged);
            // 
            // checkBoxShowFullMesh
            // 
            this.checkBoxShowFullMesh.AutoSize = true;
            this.checkBoxShowFullMesh.Location = new System.Drawing.Point(76, 44);
            this.checkBoxShowFullMesh.Name = "checkBoxShowFullMesh";
            this.checkBoxShowFullMesh.Size = new System.Drawing.Size(79, 19);
            this.checkBoxShowFullMesh.TabIndex = 3;
            this.checkBoxShowFullMesh.Text = "Full mesh";
            this.checkBoxShowFullMesh.UseVisualStyleBackColor = true;
            this.checkBoxShowFullMesh.CheckedChanged += new System.EventHandler(this.checkBoxShowFullMesh_CheckedChanged);
            // 
            // checkBoxSelectionOnly
            // 
            this.checkBoxSelectionOnly.AutoSize = true;
            this.checkBoxSelectionOnly.Location = new System.Drawing.Point(161, 44);
            this.checkBoxSelectionOnly.Name = "checkBoxSelectionOnly";
            this.checkBoxSelectionOnly.Size = new System.Drawing.Size(102, 19);
            this.checkBoxSelectionOnly.TabIndex = 3;
            this.checkBoxSelectionOnly.Text = "Selection only";
            this.checkBoxSelectionOnly.UseVisualStyleBackColor = true;
            this.checkBoxSelectionOnly.CheckedChanged += new System.EventHandler(this.checkBoxSelectionOnly_CheckedChanged);
            // 
            // checkBoxTrackable
            // 
            this.checkBoxTrackable.AutoSize = true;
            this.checkBoxTrackable.Location = new System.Drawing.Point(269, 44);
            this.checkBoxTrackable.Name = "checkBoxTrackable";
            this.checkBoxTrackable.Size = new System.Drawing.Size(79, 19);
            this.checkBoxTrackable.TabIndex = 3;
            this.checkBoxTrackable.Text = "Trackable";
            this.checkBoxTrackable.UseVisualStyleBackColor = true;
            this.checkBoxTrackable.CheckedChanged += new System.EventHandler(this.checkBoxTrackable_CheckedChanged);
            // 
            // UIWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(408, 382);
            this.Controls.Add(this.checkBoxSelectionOnly);
            this.Controls.Add(this.checkBoxTrackable);
            this.Controls.Add(this.checkBoxShowFullMesh);
            this.Controls.Add(this.checkBoxShowBoxes);
            this.Controls.Add(this.textBoxSignature);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonLoadSignature);
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
        private System.Windows.Forms.Button buttonLoadSignature;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxSignature;
        private System.Windows.Forms.CheckBox checkBoxShowBoxes;
        private System.Windows.Forms.CheckBox checkBoxShowFullMesh;
        private System.Windows.Forms.CheckBox checkBoxSelectionOnly;
        private System.Windows.Forms.CheckBox checkBoxTrackable;
    }
}

