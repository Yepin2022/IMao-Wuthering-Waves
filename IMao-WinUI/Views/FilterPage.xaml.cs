using System.Globalization;
using System.Reflection;
using System.Text.Json;
using IMao_WinUI.Helpers;
using IMao_WinUI.StringItems;
using IMao_WinUI.ViewModels;
using Microsoft.UI;
using Microsoft.UI.Text;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Shapes;
namespace IMao_WinUI.Views;

public sealed partial class FilterPage : Page
{
    private readonly StringItem stringItem = new StringItem();
    public FilterViewModel ViewModel
    {
        get;
    }

    public FilterPage()
    {
        ViewModel = App.GetService<FilterViewModel>();
        InitializeComponent();

        stringItem.LoadString(language.Text);
        GenerateCheckBoxes();
    }

    private void GenerateCheckBoxes()
    {
        foreach (var itemsDatas in stringItem.itemsDatas)
        {
            String category = itemsDatas.Category;
            TextBlock textBlock = new TextBlock
            {
                Text = category,
                FontSize = 14,
                Margin = new Thickness(10, 0, 0, 0),
                Width = 9999
            };
            CheckBoxContainer.Children.Add(textBlock);

            foreach (var itemDatas in itemsDatas.ItemDatas)
            {
                CheckBox checkBox = new CheckBox
                {
                    Content = itemDatas.Name_SpecifiedLanguage,
                    Margin = new Thickness(20, 0, 0, 0),
                    FontSize = 14,
                    HorizontalAlignment = HorizontalAlignment.Left
                };

                checkBox.Checked += CheckBox_CheckedChanged;
                checkBox.Unchecked += CheckBox_CheckedChanged;

                CheckBoxContainer.Children.Add(checkBox);
            }
        }
    }

    private void CheckBox_CheckedChanged(object sender, RoutedEventArgs e)
    {
        CheckBox checkBox = sender as CheckBox;
        if (checkBox != null)
        {
            String itemId = stringItem.GetItemID(checkBox.Content.ToString());
            if(itemId != null)
            {
                if ((bool)checkBox.IsChecked)
                {
                    IMaoCoreAPI.AddItem(itemId);
                }
                else
                {
                    IMaoCoreAPI.ClearItem(itemId);
                }
            }
        }
    }

}
