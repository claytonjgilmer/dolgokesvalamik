namespace editor
{
    partial class editorform
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
            this.propertyGrid1 = new editor.editorpropgrid();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.propertyGrid1.CommentsBackColor = System.Drawing.SystemColors.Control;
            this.propertyGrid1.CommentsForeColor = System.Drawing.SystemColors.ControlText;
            this.propertyGrid1.CommentsHeight = 40;
            this.propertyGrid1.CommentsVisibility = false;
            this.propertyGrid1.DisplayMode = VisualHint.SmartPropertyGrid.PropertyGrid.DisplayModes.Categorized;
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.EllipsisMode = VisualHint.SmartPropertyGrid.PropertyGrid.EllipsisModes.EllipsisOnValues;
            this.propertyGrid1.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            this.propertyGrid1.GridBackColor = System.Drawing.SystemColors.Window;
            this.propertyGrid1.GridDisabledForeColor = System.Drawing.SystemColors.GrayText;
            this.propertyGrid1.GridForeColor = System.Drawing.SystemColors.ControlText;
            this.propertyGrid1.HighlightedTextColor = System.Drawing.SystemColors.HighlightText;
            this.propertyGrid1.LabelColumnWidth = 150;
            this.propertyGrid1.LeftColumnWidth = 16;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid1.MultiSelectMode = VisualHint.SmartPropertyGrid.PropertyGrid.MultiSelectModes.None;
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.PropertyVerticalMargin = 4;
            this.propertyGrid1.ReadOnlyVisual = VisualHint.SmartPropertyGrid.PropertyGrid.ReadOnlyVisuals.Disabled;
            this.propertyGrid1.SelectedBkgColor = System.Drawing.SystemColors.Highlight;
            this.propertyGrid1.SelectedNotFocusedBkgColor = System.Drawing.SystemColors.ActiveBorder;
            this.propertyGrid1.Size = new System.Drawing.Size(338, 402);
            this.propertyGrid1.TabIndex = 0;
            this.propertyGrid1.ToolbarVisibility = false;
            this.propertyGrid1.ToolTipMode = VisualHint.SmartPropertyGrid.PropertyGrid.ToolTipModes.None;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Size = new System.Drawing.Size(909, 596);
            this.splitContainer1.SplitterDistance = 567;
            this.splitContainer1.TabIndex = 1;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.propertyGrid1);
            this.splitContainer2.Size = new System.Drawing.Size(338, 596);
            this.splitContainer2.SplitterDistance = 190;
            this.splitContainer2.TabIndex = 0;
            // 
            // editorform
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(909, 596);
            this.Controls.Add(this.splitContainer1);
            this.Name = "editorform";
            this.Text = "Form1";
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        editorpropgrid propertyGrid1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
    }
}

