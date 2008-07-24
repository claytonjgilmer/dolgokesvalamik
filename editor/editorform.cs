using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace editor
{
    public partial class editorform : Form
    {
        public editorform()
        {
            InitializeComponent();
            mypropgrid=new editorpropgrid();
            mypropgrid.Initialize();

        }

        ~editorform()
        {

        }

        void init_engine()
        {

        }

        editorpropgrid mypropgrid;
    }
}