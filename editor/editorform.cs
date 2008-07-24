using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using libwrapper;

namespace editor
{
    public partial class editorform : Form
    {
        public editorform()
        {
            InitializeComponent();
            property_grid.Initialize();
            property_grid.SelectedObject = this;

            libwrapper.game.init(splitContainer1.Panel1.Handle);

            this.Show();

            while (this.Created)
            {
                libwrapper.game.update();
                Application.DoEvents();
            }

            libwrapper.game.exit();

        }

        ~editorform()
        {

        }

        void init_engine()
        {

        }

    }
}