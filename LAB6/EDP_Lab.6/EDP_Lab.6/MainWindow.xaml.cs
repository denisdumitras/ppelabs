using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml;

namespace EDP_Lab._6
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void AddButton_Click(object sender, RoutedEventArgs e)
        {
            AddTaskDialog dlg = new AddTaskDialog();

            if (dlg.ShowDialog() == true)
            {
                // Get the document
                XmlDocument document = ((XmlDataProvider)FindResource("tasks")).Document;

                // Create the Task element
                XmlElement task = document.CreateElement("Task");

                // Create the Name element
                XmlElement name = document.CreateElement("Name");

                name.InnerText = dlg.TaskTitle.Text;
                task.AppendChild(name);

                // Create the Priority element
                XmlElement priority = document.CreateElement("Priority");

                priority.InnerText = dlg.TaskPriority.Text;
                task.AppendChild(priority);

                // Create the Done element
                XmlElement done = document.CreateElement("Done");

                done.InnerText = "No";
                task.AppendChild(done);

                // Create the Description element
                XmlElement description = document.CreateElement("WDescription");

                description.InnerText = dlg.TaskDescription.Text;
                task.AppendChild(description);

                document.DocumentElement.AppendChild(task);
            }
        }

        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            if (TaskListBox.SelectedIndex != -1)
            {
                XmlElement task = (XmlElement)TaskListBox.SelectedItem;
                task.ParentNode.RemoveChild(task);
            }
        }

        private void TaskListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }
    }
}
