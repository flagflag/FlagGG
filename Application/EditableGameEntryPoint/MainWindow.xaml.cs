using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;

using Windows.Foundation;
using Windows.Foundation.Collections;
using WinRT;
using WinUIEx;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace FlagGG;
/// <summary>
/// An empty window that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class MainWindow : WindowEx
{
    [LibraryImport("Game.dll", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void ShowPrefab(string prefabPath);

    public MainWindow()
    {
        this.InitializeComponent();
    }

    private void InitResourcesTree()
    {
        ResourceTree.RootNodes.Clear();
        TreeViewNode rootNode = new TreeViewNode() { Content = Path.GetDirectoryName(resourceDir_), IsExpanded = true };
        InitResourcesTreeImpl(resourceDir_, rootNode, 1);
        ResourceTree.RootNodes.Add(rootNode);
    }

    private void InitResourcesTreeImpl(string directoryPath, TreeViewNode rootNode, int depth)
    {
        string[] files = Directory.GetFiles(directoryPath, "*.*");
        foreach (string file in files)
        {
            string fileName = Path.GetFileName(file);
            if (fileName == "model.prefab")
            {
                TreeViewNode fileNode = new TreeViewNode() { Content = Path.GetFileName(directoryPath) };
                rootNode.Children.Add(fileNode);
                return;
            }
        }

        TreeViewNode dirNode = new TreeViewNode() { Content = Path.GetFileName(directoryPath), IsExpanded = depth <= 1 };
        rootNode.Children.Add(dirNode);

        string[] dirs = Directory.GetDirectories(directoryPath);
        foreach (string dir in dirs)
        {
            InitResourcesTreeImpl(dir, dirNode, depth + 1);
        }
    }

    private void OnRefreshResourceTree(object sender, RoutedEventArgs e)
    {
        InitResourcesTree();
    }

    private void OnTreeViewItem_ContextRequested(object sender, ContextRequestedEventArgs args)
    {

    }

    private void OnTreeViewItem_Click(object sender, TreeViewSelectionChangedEventArgs e)
    {
        if (ResourceTree.SelectedItems.Count > 0)
        {
            if (ResourceTree.SelectedItems[0] is TreeViewNode fileNode)
            {
                if (fileNode.Children.Count == 0)
                {
                    string prefabPath = fileNode.Content.As<string>();
                    while (fileNode.Parent != null && fileNode.Parent != ResourceTree)
                    {
                        fileNode = fileNode.Parent;
                        if (fileNode.Content == null)
                            break;
                        prefabPath = fileNode.Content.As<string>() + "\\" + prefabPath;
                    }
                    prefabPath = prefabPath + "\\model.prefab";
                    int directoryLength = Path.GetFileName(resourceDir_).Length;
                    prefabPath = prefabPath.Substring(resourceDir_.Length - directoryLength);
                    ShowPrefab(prefabPath);
                }
            }
        }
    }

    private string resourceDir_ = Environment.CurrentDirectory + "\\ResForSCE\\deco";
}
