﻿#include <neolib/neolib.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <neolib/random.hpp>
#include <neogfx/app/app.hpp>
#include <neogfx/gui/window/window.hpp>
#include <neogfx/gui/widget/push_button.hpp>
#include <neogfx/gui/layout/vertical_layout.hpp>
#include <neogfx/gui/layout/horizontal_layout.hpp>
#include <neogfx/gui/layout/grid_layout.hpp>
#include <neogfx/gui/layout/spacer.hpp>
#include <neogfx/gui/widget/table_view.hpp>
#include <neogfx/gui/widget/radio_button.hpp>
#include <neogfx/gui/widget/check_box.hpp>
#include <neogfx/gui/widget/item_model.hpp>
#include <neogfx/gui/widget/item_presentation_model.hpp>
#include <neogfx/gui/widget/tab_page_container.hpp>
#include <neogfx/hid/i_surface.hpp>
#include <neogfx/gui/widget/image_widget.hpp>
#include <neogfx/game/sprite_plane.hpp>
#include <neogfx/gui/widget/toolbar.hpp>
#include <neogfx/gui/widget/menu_bar.hpp>
#include <neogfx/gui/widget/text_edit.hpp>
#include <neogfx/gui/widget/line_edit.hpp>
#include <neogfx/gui/widget/spin_box.hpp>
#include <neogfx/gui/widget/slider.hpp>
#include <neogfx/gui/dialog/colour_dialog.hpp>
#include <neogfx/gui/widget/gradient_widget.hpp>
#include <neogfx/gui/widget/group_box.hpp>
#include <neogfx/gui/widget/drop_list.hpp>
#include <neogfx/gui/dialog/message_box.hpp>

namespace ng = neogfx;

class my_item_model : public ng::basic_item_model<void*, 9u>
{
public:
	my_item_model()
	{
		set_column_name(0, "Zero");
		set_column_name(1, "Spin Box");
		set_column_name(2, "Two");
		set_column_name(3, "Three");
		set_column_name(4, "Four");
		set_column_name(5, "Five");
		set_column_name(6, "Six");
		set_column_name(7, "Empty");
		set_column_name(8, "Eight");
	}
public:
	const ng::item_cell_data& cell_data(const ng::item_model_index& aIndex) const override
	{
		if (aIndex.column() == 4)
		{
			if (aIndex.row() == 4)
			{
				static const ng::item_cell_data sReadOnly = { "** Read Only **" };
				return sReadOnly;
			}
			if (aIndex.row() == 5)
			{
				static const ng::item_cell_data sUnselectable = { "** Unselectable **" };
				return sUnselectable;
			}
		}
		return ng::basic_item_model<void*, 9u>::cell_data(aIndex);
	}
	const ng::item_cell_data_info& cell_data_info(const ng::item_model_index& aIndex) const override
	{
		if (aIndex.column() == 4)
		{
			if (aIndex.row() == 4)
			{
				static const ng::item_cell_data_info sReadOnly = { false, true };
				return sReadOnly;
			}
			if (aIndex.row() == 5)
			{
				static const ng::item_cell_data_info sUnselectable = { true, false };
				return sUnselectable;
			}
		}
		return ng::basic_item_model<void*, 9u>::cell_data_info(aIndex);
	}
};

class my_item_presentation_model : public ng::basic_item_presentation_model<my_item_model>
{
private:
	typedef ng::basic_item_presentation_model<my_item_model> base_type;
public:
	my_item_presentation_model(my_item_model& aModel, ng::item_cell_colour_type aColourType) : base_type{ aModel }, iColourType{ aColourType }
	{
	}
public:
	ng::optional_colour cell_colour(const ng::item_presentation_model_index& aIndex, ng::item_cell_colour_type aColourType) const override
	{
		neolib::basic_random<double> prng{ (to_item_model_index(aIndex).row() << 16) + to_item_model_index(aIndex).column() }; // use seed to make random colour based on row/index
		if (aColourType == iColourType)
			return ng::hsv_color{prng(0.0, 360.0), prng(0.0, 1.0), prng(0.75, 1.0) }.to_rgb();
		else
			return iColourType == ng::item_cell_colour_type::Foreground ? ng::optional_colour{} : ng::colour::Black;
	}
private:
	ng::item_cell_colour_type iColourType;
};

class keypad_button : public ng::push_button
{
public:
	keypad_button(ng::text_edit& aTextEdit, uint32_t aNumber) :
		ng::push_button{ boost::lexical_cast<std::string>(aNumber) }, iTextEdit{ aTextEdit }
	{
		clicked([this, aNumber]()
		{
			ng::app::instance().change_style("Keypad").
				palette().set_colour(aNumber != 9 ? ng::colour{ aNumber & 1 ? 64 : 0, aNumber & 2 ? 64 : 0, aNumber & 4 ? 64 : 0 } : ng::colour::LightGoldenrod);
			if (aNumber == 9)
				iTextEdit.set_default_style(ng::text_edit::style{ ng::optional_font{}, ng::gradient{ ng::colour::DarkGoldenrod, ng::colour::LightGoldenrodYellow, ng::gradient::Horizontal }, ng::colour_or_gradient{} });
			else if (aNumber == 8)
				iTextEdit.set_default_style(ng::text_edit::style{ ng::font{"SnareDrum One NBP", "Regular", 60.0}, ng::colour::Black, ng::colour_or_gradient{}, ng::text_effect{ ng::text_effect::Outline, ng::colour::White } });
			else if (aNumber == 0)
				iTextEdit.set_default_style(ng::text_edit::style{ ng::font{"SnareDrum Two NBP", "Regular", 60.0}, ng::colour::White });
			else
				iTextEdit.set_default_style(
					ng::text_edit::style{
						ng::optional_font{},
						ng::gradient{
							ng::colour{ aNumber & 1 ? 64 : 0, aNumber & 2 ? 64 : 0, aNumber & 4 ? 64 : 0 }.lighter(0x40),
							ng::colour{ aNumber & 1 ? 64 : 0, aNumber & 2 ? 64 : 0, aNumber & 4 ? 64 : 0 }.lighter(0xC0),
							ng::gradient::Horizontal},
						ng::colour_or_gradient{} });
		});
	}
private:
	ng::text_edit& iTextEdit;
};

void create_game(ng::i_layout& aLayout);

int main(int argc, char* argv[])
{
	/* Yes this is an 800 line (and counting) function and whilst in general such long functions are
	egregious this function is a special case: it is test code which mostly just creates widgets. 
	I might break it up at a later date when I create a proper test suite. */

	ng::app app(argc, argv, "neoGFX Test App");

	try
	{
		app.set_default_window_icon(ng::image(":/test/resources/icon.png"));
		app.change_style("Default").set_font_info(ng::font_info("Segoe UI", std::string("Regular"), 9));
		app.change_style("Slate").set_font_info(ng::font_info("Segoe UI", std::string("Regular"), 9));
		app.register_style(ng::style("Keypad")).set_font_info(ng::font_info("Segoe UI", std::string("Regular"), 9));
		app.change_style("Keypad");
		app.current_style().palette().set_colour(ng::colour::Black);
		app.change_style("Default");

		ng::window window(ng::size{ 768, 688 });
		auto& layout0 = window.client_layout();

		bool showFps = false;
		auto fpsFont = window.font().with_size(18);
		window.paint_overlay([&showFps, &window, fpsFont](ng::graphics_context& aGc)
		{
			if (showFps)
			{
				std::ostringstream oss;
				oss << window.fps() << " FPS";
				aGc.fill_rect(ng::rect{ 100, 100, 128, 32 }, ng::colour::DarkBlue);
				aGc.draw_text(ng::point{ 100, 100 }, oss.str(), fpsFont, ng::colour::White);
			}
		});

		app.add_action("Goldenrod Style").set_shortcut("Ctrl+Alt+Shift+G").triggered([]()
		{
			ng::app::instance().change_style("Keypad").palette().set_colour(ng::colour::LightGoldenrod);
		});

		auto& contactsAction = app.add_action("&Contacts...", ":/closed/resources/caw_toolbar.naa#contacts.png").set_shortcut("Alt+C");
		contactsAction.triggered([]()
		{
			ng::app::instance().change_style("Keypad").palette().set_colour(ng::colour::White);
		});
		auto& muteAction = app.add_action("Mute/&Unmute Sound", ":/closed/resources/caw_toolbar.naa#mute.png");
		muteAction.set_checkable(true);
		muteAction.set_checked_image(":/closed/resources/caw_toolbar.naa#unmute.png");

		auto& pasteAndGoAction = app.add_action("Paste and Go", ":/closed/resources/caw_toolbar.naa#paste_and_go.png").set_shortcut("Ctrl+Shift+V");

		neolib::callback_timer ct{ app, [&app, &pasteAndGoAction](neolib::callback_timer& aTimer)
		{
			aTimer.again();
			if (app.clipboard().sink_active())
			{
				auto& sink = app.clipboard().active_sink();
				if (sink.can_paste())
				{
					pasteAndGoAction.enable();
				}
				else
				{
					pasteAndGoAction.disable();
				}
			}
		}, 100 };

		pasteAndGoAction.triggered([&app]()
		{
			app.clipboard().paste();
		});

		ng::menu_bar menu(window.menu_layout());

		auto& fileMenu = menu.add_sub_menu("&File");
		fileMenu.add_action(app.action_file_new());
		fileMenu.add_action(app.action_file_open());
		fileMenu.add_separator();
		fileMenu.add_action(app.action_file_close());
		fileMenu.add_separator();
		fileMenu.add_action(app.action_file_save());
		fileMenu.add_separator();
		fileMenu.add_action(app.action_file_exit());

		auto& editMenu = menu.add_sub_menu("&Edit");
		editMenu.add_action(app.action_undo());
		editMenu.add_action(app.action_redo());
		editMenu.add_separator();
		editMenu.add_action(app.action_cut());
		editMenu.add_action(app.action_copy());
		editMenu.add_action(app.action_paste());
		editMenu.add_action(pasteAndGoAction);
		editMenu.add_action(app.action_delete());
		editMenu.add_separator();
		editMenu.add_action(app.action_select_all());
		auto& viewMenu = menu.add_sub_menu("&View");
		auto& addFavouriteAction = app.add_action("Add Favourite...", ":/closed/resources/caw_toolbar.naa#add_favourite.png");
		auto& organizeFavouritesAction = app.add_action("Organize Favourites...", ":/closed/resources/caw_toolbar.naa#organize_favourites.png");

		auto& favouritesMenu = menu.add_sub_menu("F&avourites");
		favouritesMenu.add_action(addFavouriteAction);
		favouritesMenu.add_action(organizeFavouritesAction);
		favouritesMenu.add_separator();
		favouritesMenu.add_action(app.add_action("Alice", ":/closed/resources/caw_toolbar.naa#favourite.png"));
		favouritesMenu.add_action(app.add_action("Bob", ":/closed/resources/caw_toolbar.naa#favourite.png"));
		favouritesMenu.add_action(app.add_action("Carlos", ":/closed/resources/caw_toolbar.naa#favourite.png"));
		favouritesMenu.add_action(app.add_action("Dave", ":/closed/resources/caw_toolbar.naa#favourite.png"));
		auto& menuDrones = favouritesMenu.add_sub_menu("Silent Running Drones");
		menuDrones.set_image(":/closed/resources/caw_toolbar.naa#folder.png");
		menuDrones.add_action(app.add_action("Dewey", ":/closed/resources/caw_toolbar.naa#favourite.png"));
		menuDrones.add_action(app.add_action("Huey", ":/closed/resources/caw_toolbar.naa#favourite.png"));
		menuDrones.add_action(app.add_action("Louie", ":/closed/resources/caw_toolbar.naa#favourite.png"));
		auto& subMenu2 = favouritesMenu.add_sub_menu("DC Characters");
		subMenu2.set_image(":/closed/resources/caw_toolbar.naa#folder.png");
		auto& subMenu3 = favouritesMenu.add_sub_menu("Marvel Characters");
		subMenu3.set_image(":/closed/resources/caw_toolbar.naa#folder.png");
		favouritesMenu.add_separator();
		neolib::random menuPrng{ 0 };
		for (int i = 1; i <= 5; ++i)
		{
			auto& sm = favouritesMenu.add_sub_menu("More_" + boost::lexical_cast<std::string>(i));
			for (int j = 1; j <= 5; ++j)
			{
				auto& sm2 = sm.add_sub_menu("More_" + boost::lexical_cast<std::string>(i) + "_" + boost::lexical_cast<std::string>(j));
				int n = menuPrng(100);
				for (int k = 1; k < n; ++k)
				{
					sm2.add_action(app.add_action("More_" + boost::lexical_cast<std::string>(i) + "_" + boost::lexical_cast<std::string>(j) + "_" + boost::lexical_cast<std::string>(k), ":/closed/resources/caw_toolbar.naa#favourite.png"));
				}
			}
		}
		menu.add_action(contactsAction);
		menu.add_action(muteAction);
		menu.add_action(app.add_action("&Xyzzy...", ":/neogfx/resources/icons.naa#eyedropper.png"));

		auto& testMenu = menu.add_sub_menu("&Test");
		testMenu.add_action(contactsAction);
		testMenu.add_action(muteAction);
		testMenu.add_action(muteAction);
		testMenu.add_action(muteAction);
		testMenu.add_action(muteAction);
		ng::i_action& colourAction = app.add_action("Colour Dialog...");
		colourAction.triggered([&window]()
		{
			ng::color_dialog cd(window);
			cd.exec();
		});
		testMenu.add_action(colourAction);

		auto& windowMenu = menu.add_sub_menu("&Window");
		auto& nextTab = windowMenu.add_action(app.add_action("Next Tab").set_shortcut("Ctrl+Tab"));
		auto& previousTab = windowMenu.add_action(app.add_action("Previous Tab").set_shortcut("Shift+Ctrl+Tab"));

		auto& helpMenu = menu.add_sub_menu("&Help");

		ng::toolbar toolbar(window.toolbar_layout());
		toolbar.add_action(app.action_file_new());
		toolbar.add_action(app.action_file_open());
		toolbar.add_action(app.action_file_save());
		toolbar.add_separator();
		toolbar.add_action(contactsAction);
		toolbar.add_action(addFavouriteAction);
		toolbar.add_action(organizeFavouritesAction);
		toolbar.add_action(app.add_action("Keywords...", ":/closed/resources/caw_toolbar.naa#keyword.png"));
		toolbar.add_action(app.add_action("Settings...", ":/closed/resources/caw_toolbar.naa#settings.png"));
		toolbar.add_action(app.add_action("Manage Plugins...", ":/closed/resources/caw_toolbar.naa#manage_plugins.png"));
		toolbar.add_action(muteAction);
		toolbar.add_separator();
		toolbar.add_action(app.action_cut());
		toolbar.add_action(app.action_copy());
		toolbar.add_action(app.action_paste());
		toolbar.add_action(pasteAndGoAction);
		toolbar.add_separator();
		toolbar.add_action(app.add_action("Check for Updates...", ":/closed/resources/caw_toolbar.naa#setup.png"));

		ng::tab_page_container tabContainer(layout0, true);

		nextTab.triggered([&]() { tabContainer.select_next_tab(); });
		previousTab.triggered([&]() { tabContainer.select_previous_tab(); });

		// Buttons

		ng::i_widget& buttonsPage = tabContainer.add_tab_page("Buttons").as_widget();
		ng::vertical_layout layoutButtons(buttonsPage);
		layoutButtons.set_margins(ng::margins(8));
		ng::horizontal_layout topButtons(layoutButtons);
		ng::push_button button0(topButtons, "This is the neoGFX test application.");
		button0.label().set_placement(ng::label_placement::ImageTextVertical);
		button0.image().set_image(ng::image{ ":/test/resources/neoGFX.png" });
		button0.image().set_minimum_size(ng::size{ 32, 32 });
		button0.image().set_maximum_size(ng::size{ 160, std::numeric_limits<ng::dimension>::max() });
		button0.set_size_policy(ng::size_policy::Expanding);
		button0.set_foreground_colour(ng::colour::LightGoldenrodYellow);
		ng::push_button button1(topButtons, "the,,, quick brown fox jumps over the lazy dog");
		ng::push_button buttonGenerateUuid(topButtons, "Generate UUID");
		ng::horizontal_layout international(layoutButtons);
		button1.set_foreground_colour(ng::colour::LightGoldenrod);
		ng::push_button button2(international, u8"ويقفز الثعلب البني السريع فوق الكلب الكسول");
		button2.set_foreground_colour(ng::colour::Goldenrod);
		ng::push_button button3(international, u8"שועל חום קפיצות מעל הכלב העצלן");
		button3.set_foreground_colour(ng::colour::DarkGoldenrod);
		ng::push_button button4(international, u8"请停止食用犬");
		button4.clicked([&window]() { window.set_title_text(u8"请停止食用犬"); });
		button4.set_foreground_colour(ng::colour::CadetBlue);
		button4.set_maximum_size(ng::size(128, 64));
		ng::push_button button5(layoutButtons, u8"sample te&xt نص عينة sample text טקסט לדוגמא 示例文本 sample text\nKerning test: Tr. WAVAVAW. zzz zoz ozo ooo");
		ng::horizontal_layout dropListLayout(layoutButtons);
		ng::drop_list dropList(dropListLayout);
		ng::drop_list dropList2(dropListLayout);
		dropList2.set_editable(true);
		ng::horizontal_layout editLayout(layoutButtons);
		ng::text_edit textEdit(editLayout);
		buttonGenerateUuid.clicked([&textEdit]() { textEdit.set_text(neolib::to_string(neolib::generate_uuid())); });
		textEdit.set_focus_policy(textEdit.focus_policy() | neogfx::focus_policy::ConsumeTabKey);
		textEdit.set_tab_stop_hint("00000000");
		ng::slider effectWidthSlider{ editLayout, ng::slider::Vertical };
		effectWidthSlider.set_minimum(1);
		effectWidthSlider.set_maximum(10);
		effectWidthSlider.set_step(1);
		effectWidthSlider.set_value(5);
		ng::double_slider effectAux1Slider{ editLayout, ng::slider::Vertical };
		effectAux1Slider.set_minimum(0.1);
		effectAux1Slider.set_maximum(10.0);
		effectAux1Slider.set_step(0.1);
		effectAux1Slider.set_value(1.0);
		ng::text_edit smallTextEdit(editLayout);
		smallTextEdit.set_maximum_width(100);
		smallTextEdit.set_alignment(ng::alignment::Right);
		ng::horizontal_layout layoutLineEdits(layoutButtons);
		ng::line_edit lineEdit(layoutLineEdits);
		lineEdit.set_text("Line edit");
		ng::line_edit lineEditPassword(layoutLineEdits);
		lineEditPassword.set_text("Password");
		ng::double_spin_box spinBox(layoutLineEdits);
		spinBox.set_minimum(-100.0);
		spinBox.set_maximum(100.0);
		spinBox.set_step(0.1);
		ng::double_slider slider(layoutLineEdits);
		slider.set_minimum(-100.0);
		slider.set_maximum(100.0);
		slider.set_step(0.1);
		spinBox.value_changed([&slider, &spinBox]() {slider.set_value(spinBox.value()); });
		ng::double_spin_box doubleSpinBox(layoutLineEdits);
		doubleSpinBox.set_minimum(-10);
		doubleSpinBox.set_maximum(20);
		doubleSpinBox.set_step(0.5);
		ng::horizontal_layout layout2(layoutButtons);
		ng::label label1(layout2, "Label 1:");
		bool colourCycle = true;
		ng::push_button button6(layout2, "RGB <-> HSV\ncolour space\nconversion test");
		button6.clicked([&colourCycle]() { colourCycle = !colourCycle; });
		layout2.add_spacer().set_weight(ng::size(2.0f));
		ng::push_button button7(layout2, "Toggle\n&mute.");
		button7.set_foreground_colour(ng::colour::LightCoral);
		button7.set_maximum_size(ng::size(128, 64));
		button7.clicked([&muteAction]() { muteAction.toggle(); });
		layout2.add_spacer().set_weight(ng::size(1.0));
		ng::push_button button8(layout2, "Enable/disable\ncontacts action.");
		button8.set_foreground_colour(ng::colour(255, 235, 160));
		button8.clicked([&contactsAction]() { if (contactsAction.is_enabled()) contactsAction.disable(); else contactsAction.enable(); });
		ng::horizontal_layout layout3(layoutButtons);
		neolib::random prng{ 3 };
		for (uint32_t i = 0; i < 10; ++i)
		{
			auto button = std::make_shared<ng::push_button>(std::string(1, 'A' + i));
			layout3.add_item(button);
			ng::colour randomColour = ng::colour{ prng(255), prng(255), prng(255) };
			button->set_foreground_colour(randomColour);
		}
		ng::group_box groupBox{ layout2, "Group Box" };
		ng::vertical_layout& layoutRadiosAndChecks = static_cast<ng::vertical_layout&>(groupBox.item_layout());
		ng::check_box triState(layoutRadiosAndChecks, "Tristate checkbox", ng::check_box::TriState);
		auto showHideTabs = [&triState, &tabContainer]()
		{
			if (triState.is_checked())
				tabContainer.hide_tab(8);
			else
				tabContainer.show_tab(8);
			if (triState.is_indeterminate())
				tabContainer.hide_tab(9);
			else
				tabContainer.show_tab(9);
		};
		triState.checked([&triState, showHideTabs]()
		{
			static uint32_t n;
			if ((n++)%2 == 1)
				triState.set_indeterminate();
			showHideTabs();
		});
		triState.unchecked([&triState, showHideTabs]()
		{
			showHideTabs();
		});
		ng::check_box wordWrap(layoutRadiosAndChecks, "Editor word wrap");
		wordWrap.check();
		wordWrap.checked([&textEdit]()
		{
			textEdit.set_word_wrap(true);
		});
		wordWrap.unchecked([&textEdit]()
		{
			textEdit.set_word_wrap(false);
		});
		ng::check_box password(layoutRadiosAndChecks, "Password");
		password.checked([&lineEditPassword]()
		{
			lineEditPassword.set_password(true);
		});
		password.unchecked([&lineEditPassword]()
		{
			lineEditPassword.set_password(false);
		});
		ng::check_box columns(layoutRadiosAndChecks, "Columns");
		ng::check_box groupBoxCheckable(layoutRadiosAndChecks, "Group Box Checkable");
		groupBoxCheckable.checked([&showFps, &groupBox]()
		{
			showFps = true;
			groupBox.set_checkable(true);
		});
		groupBoxCheckable.unchecked([&showFps, &groupBox]()
		{
			showFps = false;
			groupBox.set_checkable(false);
		});
		ng::gradient_widget gw(layoutRadiosAndChecks);
		columns.checked([&textEdit, &gw, &password]()
		{
			password.disable();
			textEdit.set_columns(3);
			gw.gradient_changed([&gw, &textEdit]()
			{
				auto cs = textEdit.column(2);
				cs.set_style(ng::text_edit::style{ ng::optional_font{}, ng::colour_or_gradient{}, ng::colour_or_gradient{}, ng::text_effect{ ng::text_effect::Outline, gw.gradient() } });
				textEdit.set_column(2, cs);
			}, textEdit);
		});
		columns.unchecked([&textEdit, &gw, &password]()
		{
			password.enable();
			textEdit.remove_columns();
			gw.gradient_changed.unsubscribe(textEdit);
		});
		ng::vertical_spacer spacerCheckboxes(layoutRadiosAndChecks);
		ng::vertical_layout layout4(layout2);
		ng::horizontal_layout layout7(layout4);
		ng::check_box buttonKerning(layout7, "kern");
		ng::check_box buttonSubpixel(layout7, "subpix");
		buttonKerning.toggled([&app, &buttonKerning]()
		{
			auto fi = app.current_style().font_info();
			if (buttonKerning.is_checked())
				fi.enable_kerning();
			else
				fi.disable_kerning();
			app.current_style().set_font_info(fi);
		});
		buttonSubpixel.toggled([&app, &buttonSubpixel]()
		{
			if (buttonSubpixel.is_checked())
				app.rendering_engine().subpixel_rendering_on();
			else
				app.rendering_engine().subpixel_rendering_off();
		});
		buttonSubpixel.check();
		ng::horizontal_layout layoutColourPickers{ layout4 };
		ng::push_button themeColour(layoutColourPickers, "theme"); themeColour.image().set_image(ng::image{ ":/closed/resources/caw_toolbar.naa#colour.png" });
		ng::push_button editColour(layoutColourPickers, "edit"); editColour.image().set_image(ng::image{ ":/closed/resources/caw_toolbar.naa#colour.png" });
		ng::widget lw{ layout4 };
		lw.set_margins(ng::margins{});
		ng::grid_layout layoutEffects{ lw, 2, 2 };
		layoutEffects.set_margins(ng::margins{});
		ng::radio_button editNormal{ layoutEffects, "normal" };
		ng::radio_button editOutline{ layoutEffects, "outline" };
		ng::radio_button editGlow{ layoutEffects, "glow" };
		ng::radio_button editShadow{ layoutEffects, "shadow" };
		editNormal.checked([&]()
		{
			auto s = textEdit.default_style();
			s.set_text_effect(ng::optional_text_effect{});
			textEdit.set_default_style(s);
		});
		editOutline.checked([&]()
		{
			auto s = textEdit.default_style();
			s.set_text_effect(ng::text_effect{ ng::text_effect::Outline, ng::colour::White });
			textEdit.set_default_style(s);
		});
		editGlow.checked([&]()
		{
			auto s = textEdit.default_style();
			s.set_text_effect(ng::text_effect{ ng::text_effect::Glow, ng::colour::Orange.with_lightness(0.9) });
			textEdit.set_default_style(s);
		});
		effectWidthSlider.value_changed([&]()
		{
			editGlow.check();
			auto s = textEdit.default_style();
			s.set_text_effect(ng::text_effect{ ng::text_effect::Glow, s.text_effect()->colour(), effectWidthSlider.value(), s.text_effect()->aux1() });
			textEdit.set_default_style(s);
			std::ostringstream oss;
			oss << effectWidthSlider.value() << std::endl << effectAux1Slider.value() << std::endl;
			smallTextEdit.set_text(oss.str());
		});
		effectAux1Slider.value_changed([&]()
		{
			editGlow.check();
			auto s = textEdit.default_style();
			s.set_text_effect(ng::text_effect{ ng::text_effect::Glow, s.text_effect()->colour(), s.text_effect()->width(), effectAux1Slider.value() });
			textEdit.set_default_style(s);
			std::ostringstream oss;
			oss << effectWidthSlider.value() << std::endl << effectAux1Slider.value() << std::endl;
			smallTextEdit.set_text(oss.str());
		});
		editShadow.checked([&]()
		{
			auto s = textEdit.default_style();
			s.set_text_effect(ng::text_effect{ ng::text_effect::Shadow, ng::colour::Black });
			textEdit.set_default_style(s);
		});
		ng::radio_button radio1(layout4, "Radio 1");
		ng::radio_button radioSliderFont(layout4, "Slider changes\nfont size");
		radioSliderFont.checked([&slider, &app]()
		{
			app.current_style().set_font_info(app.current_style().font_info().with_size(slider.normalized_value() * 18.0 + 4));
		});
		ng::radio_button radio3(layout4, "Radio 3");
		radio3.disable();
		ng::radio_button radioThemeColour(layout4, "Slider changes\ntheme colour");
		auto update_theme_colour = [&slider]()
		{
			auto themeColour = ng::app::instance().current_style().palette().colour().to_hsv();
			themeColour.set_hue(slider.normalized_value() * 360.0);
			ng::app::instance().current_style().palette().set_colour(themeColour.to_rgb());
		};
		slider.value_changed([update_theme_colour, &slider, &radioSliderFont, &radioThemeColour, &spinBox, &app]()
		{
			spinBox.set_value(slider.value());
			if (radioSliderFont.is_checked())
				app.current_style().set_font_info(app.current_style().font_info().with_size(slider.normalized_value() * 18.0 + 4));
			else if (radioThemeColour.is_checked())
				update_theme_colour();
		});
		slider.set_normalized_value((app.current_style().font_info().size() - 4) / 18.0);
		radioThemeColour.checked([update_theme_colour, &slider, &app]()
		{
			slider.set_normalized_value(ng::app::instance().current_style().palette().colour().to_hsv().hue() / 360.0);
			update_theme_colour();
		});

		themeColour.clicked([&window]()
		{
			static boost::optional<ng::colour_dialog::custom_colour_list> sCustomColours;
			if (sCustomColours == boost::none)
			{
				sCustomColours = ng::colour_dialog::custom_colour_list{};
				std::fill(sCustomColours->begin(), sCustomColours->end(), ng::colour::White);
			}
			ng::colour_dialog colourPicker(window, ng::app::instance().change_style("Keypad").palette().colour());
			colourPicker.custom_colours() = *sCustomColours;
			if (colourPicker.exec() == ng::dialog_result::Accepted)
				ng::app::instance().change_style("Keypad").palette().set_colour(colourPicker.selected_colour());
			*sCustomColours = colourPicker.custom_colours();
		});

		editColour.clicked([&]()
		{
			static boost::optional<ng::colour_dialog::custom_colour_list> sCustomColours;
			if (sCustomColours == boost::none)
			{
				sCustomColours = ng::colour_dialog::custom_colour_list{};
				std::fill(sCustomColours->begin(), sCustomColours->end(), ng::colour::White);
			}
			static ng::colour sInk = ng::app::instance().current_style().palette().text_colour();
			ng::colour_dialog colourPicker(window, sInk);
			colourPicker.custom_colours() = *sCustomColours;
			if (colourPicker.exec() == ng::dialog_result::Accepted)
			{
				sInk = colourPicker.selected_colour();
				textEdit.set_default_style(ng::text_edit::style{ ng::optional_font{}, ng::gradient{ sInk, ng::colour::White, ng::gradient::Horizontal }, ng::colour_or_gradient{} }, true);
				lineEdit.set_default_style(ng::text_edit::style{ ng::optional_font{}, ng::gradient{ sInk, ng::colour::White, ng::gradient::Horizontal }, ng::colour_or_gradient{} }, true);
				lineEditPassword.set_default_style(ng::text_edit::style{ ng::optional_font{}, ng::gradient{ sInk, ng::colour::White, ng::gradient::Horizontal }, ng::colour_or_gradient{} }, true);
			}
			*sCustomColours = colourPicker.custom_colours();
		});

		ng::vertical_spacer spacer1{ layout4 };
		ng::vertical_layout keypadLayout{ layout2 };
		ng::push_button button9(keypadLayout, "Default/Slate\nStyle");
		button9.clicked([&app]()
		{
			if (app.current_style().name() == "Default")
				app.change_style("Slate");
			else
				app.change_style("Default");
		});
		button9.set_foreground_colour(ng::colour::Aquamarine);
		ng::grid_layout keypad{ keypadLayout, 4, 3 };
		keypad.set_minimum_size(ng::size{ 100.0, 0.0 });
		keypad.set_spacing(0.0);
		for (uint32_t row = 0; row < 3; ++row)
			for (uint32_t col = 0; col < 3; ++col)
				keypad.add_item_at_position(row, col, std::make_shared<keypad_button>(textEdit, row * 3 + col + 1));
		keypad.add_item_at_position(3, 1, std::make_shared<keypad_button>(textEdit, 0));
		keypad.add_span(3, 1, 1, 2);

		neolib::callback_timer animation(app, [&](neolib::callback_timer& aTimer)
		{
			if (button6.is_singular())
				return;
			aTimer.again();
			if (colourCycle)
			{
				const double PI = 2.0 * std::acos(0.0);
				double brightness = ::sin((app.program_elapsed_ms() / 16 % 360) * (PI / 180.0)) / 2.0 + 0.5;
				neolib::random prng{ app.program_elapsed_ms() / 5000 };
				ng::colour randomColour = ng::colour{ prng(255), prng(255), prng(255) };
				randomColour = randomColour.to_hsv().with_brightness(brightness).to_rgb();
				button6.set_foreground_colour(randomColour);
			}
		}, 16);

		ng::i_widget& mdiPage = tabContainer.add_tab_page("MDI").as_widget();
		app.action_file_new().triggered([&]()
		{

		});

		ng::i_widget& messageBoxesPage = tabContainer.add_tab_page("Message Boxes").as_widget();
		ng::horizontal_layout messageBoxesPageLayout1{ messageBoxesPage };
		ng::group_box messageBoxIconsGroup{ messageBoxesPageLayout1, "Icons" };
		ng::radio_button messageBoxIconInformation{ messageBoxIconsGroup.item_layout(), "Information" };
		ng::radio_button messageBoxIconQuestion{ messageBoxIconsGroup.item_layout(), "Question" };
		ng::radio_button messageBoxIconWarning{ messageBoxIconsGroup.item_layout(), "Warning" };
		ng::radio_button messageBoxIconStop{ messageBoxIconsGroup.item_layout(), "Stop" };
		ng::radio_button messageBoxIconError{ messageBoxIconsGroup.item_layout(), "Error" };
		ng::radio_button messageBoxIconCritical{ messageBoxIconsGroup.item_layout(), "Critical" };
		messageBoxIconInformation.label().image().set_image(ng::image{ ":/neogfx/resources/icons.naa#information.png" }); 
		messageBoxIconQuestion.label().image().set_image(ng::image{ ":/neogfx/resources/icons.naa#question.png" });
		messageBoxIconWarning.label().image().set_image(ng::image{ ":/neogfx/resources/icons.naa#warning.png" });
		messageBoxIconStop.label().image().set_image(ng::image{ ":/neogfx/resources/icons.naa#stop.png" });
		messageBoxIconError.label().image().set_image(ng::image{ ":/neogfx/resources/icons.naa#error.png" });
		messageBoxIconCritical.label().image().set_image(ng::image{ ":/neogfx/resources/icons.naa#critical.png" });
		messageBoxIconInformation.label().image().set_fixed_size(ng::size{ 24.0 });
		messageBoxIconQuestion.label().image().set_fixed_size(ng::size{ 24.0 });
		messageBoxIconWarning.label().image().set_fixed_size(ng::size{ 24.0 });
		messageBoxIconStop.label().image().set_fixed_size(ng::size{ 24.0 });
		messageBoxIconError.label().image().set_fixed_size(ng::size{ 24.0 });
		messageBoxIconCritical.label().image().set_fixed_size(ng::size{ 24.0 });
		ng::group_box messageBoxButtonsGroup{ messageBoxesPageLayout1, "Buttons" };
		uint32_t standardButtons = 0;
		uint32_t standardButton = 1;
		while (standardButton != 0)
		{
			auto bd = ng::dialog_button_box::standard_button_details(static_cast<ng::standard_button>(standardButton));
			if (bd.first != ng::button_role::Invalid)
			{
				auto b = std::make_shared<ng::check_box>(bd.second);
				messageBoxButtonsGroup.item_layout().add_item(b);
				b->checked([&standardButtons, standardButton]() { standardButtons |= standardButton; });
				b->unchecked([&standardButtons, standardButton]() { standardButtons &= ~standardButton; });
			}
			standardButton <<= 1;
		}
		ng::group_box messageBoxTextGroup{ messageBoxesPageLayout1, "Text" };
		ng::label messageBoxTitleLabel{ messageBoxTextGroup.item_layout(), "Title:" };
		ng::line_edit messageBoxTitle{ messageBoxTextGroup.item_layout() };
		messageBoxTitle.set_text("neoGFX Message Box Test");
		ng::label messageBoxTextLabel{ messageBoxTextGroup.item_layout(), "Text:" };
		ng::text_edit messageBoxText{ messageBoxTextGroup.item_layout() };
		messageBoxText.set_text("This is a test of the neoGFX message box.\nThis is a line of text.\n\nThis is a line of text after a blank line");
		messageBoxText.set_minimum_size(ng::size{ 256, 128 });
		ng::label messageBoxDetailedTextLabel{ messageBoxTextGroup.item_layout(), "Detailed Text:" };
		ng::text_edit messageBoxDetailedText{ messageBoxTextGroup.item_layout() };
		messageBoxDetailedText.set_text("This is where optional informative text usually goes.\nThis is a line of text.\n\nThe previous line was intentionally left blank.");
		messageBoxDetailedText.set_minimum_size(ng::size{ 256, 128 });
		ng::push_button openMessageBox{ messageBoxesPageLayout1, "Open Message Box" };
		openMessageBox.set_size_policy(ng::size_policy::Minimum);
		ng::label messageBoxResult{ messageBoxesPageLayout1 };
		openMessageBox.clicked([&]()
		{
			ng::standard_button result;
			if (messageBoxIconInformation.is_checked())
				result = ng::message_box::information(window, messageBoxTitle.text(), messageBoxText.text(), messageBoxDetailedText.text(), static_cast<ng::standard_button>(standardButtons));
			else if (messageBoxIconQuestion.is_checked())
				result = ng::message_box::question(window, messageBoxTitle.text(), messageBoxText.text(), messageBoxDetailedText.text(), static_cast<ng::standard_button>(standardButtons));
			else if (messageBoxIconWarning.is_checked())
				result = ng::message_box::warning(window, messageBoxTitle.text(), messageBoxText.text(), messageBoxDetailedText.text(), static_cast<ng::standard_button>(standardButtons));
			else if (messageBoxIconStop.is_checked())
				result = ng::message_box::stop(window, messageBoxTitle.text(), messageBoxText.text(), messageBoxDetailedText.text(), static_cast<ng::standard_button>(standardButtons));
			else if (messageBoxIconError.is_checked())
				result = ng::message_box::error(window, messageBoxTitle.text(), messageBoxText.text(), messageBoxDetailedText.text(), static_cast<ng::standard_button>(standardButtons));
			else if (messageBoxIconCritical.is_checked())
				result = ng::message_box::critical(window, messageBoxTitle.text(), messageBoxText.text(), messageBoxDetailedText.text(), static_cast<ng::standard_button>(standardButtons));
			messageBoxResult.text().set_text("Result = " + ng::dialog_button_box::standard_button_details(result).second);
		});

		// Item Views

		app.window_manager().save_mouse_cursor();
		app.window_manager().set_mouse_cursor(ng::mouse_system_cursor::Wait);

		ng::i_widget& itemViewsPage = tabContainer.add_tab_page("Item Views").as_widget();
		ng::vertical_layout layoutItemViews(itemViewsPage);
		ng::table_view tableView1(layoutItemViews);
		ng::table_view tableView2(layoutItemViews);
		tableView1.set_minimum_size(ng::size(128, 128));
		tableView2.set_minimum_size(ng::size(128, 128));
		ng::push_button button10(layoutItemViews, "Toggle List\nHeader View");
		button10.clicked([&tableView1, &tableView2]()
		{
			if (tableView1.column_header().visible())
				tableView1.column_header().hide();
			else
				tableView1.column_header().show();
			if (tableView2.column_header().visible())
				tableView2.column_header().hide();
			else
				tableView2.column_header().show();
		});
		ng::horizontal_layout tableViewTweaks(layoutItemViews);
		tableViewTweaks.set_alignment(ng::alignment::Top);
		tableViewTweaks.add_spacer();
		ng::vertical_layout tableViewSelection(tableViewTweaks);
		ng::radio_button noSelection(tableViewSelection, "No selection");
		ng::radio_button singleSelection(tableViewSelection, "Single selection");
		ng::radio_button multipleSelection(tableViewSelection, "Multiple selection");
		ng::radio_button extendedSelection(tableViewSelection, "Extended selection");
		ng::group_box column5(tableViewTweaks, "Column 5");
		ng::check_box column5ReadOnly(column5.item_layout(), "Read only");
		ng::check_box column5Unselectable(column5.item_layout(), "Unselectable");
		ng::vertical_spacer column5Spacer(column5.item_layout());
		tableViewTweaks.add_spacer();

		my_item_model itemModel;
		#ifdef NDEBUG
		itemModel.reserve(500);
		#else
		itemModel.reserve(100);
		#endif
		ng::app::event_processing_context epc(app);
		for (uint32_t row = 0; row < itemModel.capacity(); ++row)
		{
			#ifdef NDEBUG
			if (row % 100 == 0)
				app.process_events(epc);
			#else
			if (row % 10 == 0)
				app.process_events(epc);
			#endif
			neolib::random prng;
			for (uint32_t col = 0; col < 9; ++col)
			{
				if (col == 0)
					itemModel.insert_item(ng::item_model_index(row), row + 1);
				else if (col == 1)
					itemModel.insert_cell_data(ng::item_model_index(row, col), row + 1);
				else if (col != 7)
				{
					std::string randomString;
					for (uint32_t j = prng(12); j-- > 0;)
						randomString += static_cast<char>('A' + prng('z' - 'A'));
					itemModel.insert_cell_data(ng::item_model_index(row, col), randomString);
				}
			}
		} 

		auto update_column5_heading = [&]()
		{
			std::string heading;
			if (itemModel.column_read_only(5))
				heading = "Read only";
			if (!itemModel.column_selectable(5))
			{
				if (!heading.empty())
					heading += "/\n";
				heading += "Unselectable";
			}
			if (heading.empty())
				heading = "Five";
			else
				heading = "*****" + heading + "*****";
			itemModel.set_column_name(5, heading);
		};

		column5ReadOnly.checked([&]() {	itemModel.set_column_read_only(5, true); update_column5_heading(); });
		column5ReadOnly.unchecked([&]() { itemModel.set_column_read_only(5, false); update_column5_heading(); });
		column5Unselectable.checked([&]() { itemModel.set_column_selectable(5, false); update_column5_heading(); });
		column5Unselectable.unchecked([&]() { itemModel.set_column_selectable(5, true); update_column5_heading(); });

		itemModel.set_column_min_value(0, 0);
		itemModel.set_column_max_value(0, 9999);
		itemModel.set_column_min_value(1, 0);
		itemModel.set_column_max_value(1, 9999);
		itemModel.set_column_step_value(1, 1);
		tableView1.set_model(itemModel);
		my_item_presentation_model ipm1{ itemModel, ng::item_cell_colour_type::Foreground };
		tableView1.set_presentation_model(ipm1);
		ipm1.set_column_editable(4, ng::item_cell_editable::WhenFocused);
		ipm1.set_column_editable(5, ng::item_cell_editable::WhenFocused);
		ipm1.set_column_editable(6, ng::item_cell_editable::WhenFocused);
		ipm1.set_column_editable(7, ng::item_cell_editable::WhenFocused);
		ipm1.set_column_editable(8, ng::item_cell_editable::WhenFocused);
		tableView2.set_model(itemModel);
		my_item_presentation_model ipm2{ itemModel, ng::item_cell_colour_type::Background };
		ipm2.set_column_editable(0, ng::item_cell_editable::WhenFocused);
		ipm2.set_column_editable(1, ng::item_cell_editable::WhenFocused);
		ipm2.set_column_editable(2, ng::item_cell_editable::WhenFocused);
		ipm2.set_column_editable(3, ng::item_cell_editable::WhenFocused);
		tableView2.set_presentation_model(ipm2);
		tableView2.column_header().set_expand_last_column(true);
		tableView1.keyboard_event([&tableView1](const ng::keyboard_event& ke)
		{
			if (ke.type() == ng::keyboard_event_type::KeyPressed && ke.scan_code() == ng::ScanCode_DELETE && tableView1.model().rows() > 0 && tableView1.selection_model().has_current_index())
				tableView1.model().remove_item(tableView1.model().begin() + tableView1.presentation_model().to_item_model_index(tableView1.selection_model().current_index()).row());
		});

		app.window_manager().restore_mouse_cursor(window);

		auto& w = tabContainer.add_tab_page("Lots").as_widget();
		ng::vertical_layout l(w);
		#ifdef NDEBUG
		for (int i = 0; i < 1000; ++i)
		#else
		for (int i = 0; i < 100; ++i)
		#endif
			l.add_item(std::make_shared<ng::push_button>(boost::lexical_cast<std::string>(i)));
		auto& w2 = tabContainer.add_tab_page("Images").as_widget();
		ng::horizontal_layout l2(w2);
		ng::vertical_layout l3(l2);
		ng::image_widget iw(l3, ng::image(":/test/resources/channel_256.png"), ng::aspect_ratio::Ignore);
		iw.set_background_colour(ng::colour::Red.lighter(0x80));
		iw.set_size_policy(ng::size_policy::Expanding);
		iw.set_minimum_size(ng::size{});
		ng::image_widget iw2(l3, ng::image(":/test/resources/channel_256.png"), ng::aspect_ratio::Keep);
		iw2.set_background_colour(ng::colour::Green.lighter(0x80));
		iw2.set_size_policy(ng::size_policy::Expanding);
		iw2.set_minimum_size(ng::size{});
		ng::image_widget iw3(l3, ng::image(":/test/resources/channel_256.png"), ng::aspect_ratio::KeepExpanding);
		iw3.set_background_colour(ng::colour::Blue.lighter(0x80));
		iw3.set_size_policy(ng::size_policy::Expanding);
		iw3.set_minimum_size(ng::size{});
		ng::image_widget iw4(l3, ng::image(":/test/resources/channel_256.png"), ng::aspect_ratio::Stretch);
		iw4.set_background_colour(ng::colour::Magenta.lighter(0x80));
		iw4.set_size_policy(ng::size_policy::Expanding);
		iw4.set_minimum_size(ng::size{});
		ng::image_widget iw5(l2, ng::image(":/test/resources/orca.png"));
		ng::grid_layout l4(l2);
		l4.set_spacing(ng::size{});
		ng::image hash(":/test/resources/channel_32.png");
		for (uint32_t i = 0; i < 9; ++i)
		{
			auto hashWidget = std::make_shared<ng::image_widget>(hash, ng::aspect_ratio::Keep, static_cast<ng::cardinal_placement>(i));
			hashWidget->set_size_policy(ng::size_policy::Expanding);
			hashWidget->set_background_colour(i % 2 == 0 ? ng::colour::Black : ng::colour::White);
			l4.add_item_at_position(i / 3, i % 3, hashWidget);
		}
		ng::image smallHash(":/test/resources/channel.png");

		auto& gamePage = tabContainer.add_tab_page("Game").as_widget();
		ng::vertical_layout gl(gamePage);
		create_game(gl);

		auto& tabDrawing = tabContainer.add_tab_page("Drawing").as_widget();
		tabDrawing.painting([&tabDrawing, &gw](ng::graphics_context& aGc)
		{
			ng::texture logo{ ng::image{ ":/test/resources/neoGFX.png" } };
			aGc.draw_texture(ng::point{ (tabDrawing.extents() - logo.extents()) / 2.0 }, logo);
			aGc.fill_rounded_rect(ng::rect{ 100, 100, 100, 100 }, 10.0, ng::colour::Goldenrod);
			aGc.fill_rect(ng::rect{ 300, 250, 200, 100 }, gw.gradient().with_direction(ng::gradient::Horizontal));
			aGc.fill_rounded_rect(ng::rect{ 300, 400, 200, 100 }, 10.0, gw.gradient().with_direction(ng::gradient::Horizontal));
			aGc.draw_rounded_rect(ng::rect{ 300, 400, 200, 100 }, 10.0, ng::pen{ ng::colour::Blue4, 2.0 });
			aGc.draw_rounded_rect(ng::rect{ 150, 150, 300, 300 }, 10.0, ng::pen{ ng::colour::Red4, 2.0 });
			aGc.fill_rounded_rect(ng::rect{ 500, 500, 200, 200 }, 10.0, gw.gradient().with_direction(ng::gradient::Radial));
			aGc.draw_rounded_rect(ng::rect{ 500, 500, 200, 200 }, 10.0, ng::pen{ ng::colour::Black, 1.0 });
			aGc.fill_arc(ng::point{ 500, 50 }, 75, 0.0, ng::to_rad(45.0), ng::colour::Chocolate);
			aGc.draw_arc(ng::point{ 500, 50 }, 75, 0.0, ng::to_rad(45.0), ng::pen{ ng::colour::White, 3.0 });
			aGc.draw_arc(ng::point{ 500, 50 }, 50, ng::to_rad(5.0), ng::to_rad(40.0), ng::pen{ ng::colour::Yellow, 3.0 });
			for (int x = 0; x < 10; ++x)
				for (int y = 0; y < 10; ++y)
					if ((x + y % 2) % 2 == 0)
						aGc.draw_pixel(ng::point{ 10.0 + x, 10.0 + y }, ng::colour::Black);
					else
						aGc.set_pixel(ng::point{ 10.0 + x, 10.0 + y }, ng::colour::Goldenrod);
		});
		
		auto& tabEditor = tabContainer.add_tab_page("Editor").as_widget();
		ng::vertical_layout layoutEditor(tabEditor);
		ng::text_edit textEdit2(layoutEditor);
		textEdit2.set_default_style(ng::text_edit::style(ng::optional_font(), ng::gradient(ng::colour::Red, ng::colour::White, ng::gradient::Horizontal), ng::colour_or_gradient()));
		ng::push_button editorStyle1(layoutEditor, "Style 1");
		editorStyle1.clicked([&textEdit2]()
		{
			textEdit2.set_default_style(ng::text_edit::style(ng::optional_font(), ng::gradient(ng::colour::Red, ng::colour::White, ng::gradient::Horizontal), ng::colour_or_gradient()));
		});
		ng::push_button editorStyle2(layoutEditor, "Style 2");
		editorStyle2.clicked([&textEdit2]()
		{
			textEdit2.set_default_style(ng::text_edit::style(ng::font("SnareDrum One NBP", "Regular", 60.0), ng::colour::White));
		});

		auto& circlesWidget = tabContainer.add_tab_page("Circles").as_widget();
		circlesWidget.painting([&circlesWidget](ng::graphics_context& aGc)
		{
			neolib::basic_random<ng::coordinate> prng;
			neolib::basic_random<uint8_t> rngColour;
			auto random_colour = [&]()
			{
				return ng::colour{ rngColour(255), rngColour(255), rngColour(255) };
			};
			for (int i = 0; i < 100; ++i)
			{
				switch (static_cast<int>(prng(2)))
				{
				case 0:
					aGc.draw_circle(
						ng::point{ prng(circlesWidget.client_rect().cx - 1), prng(circlesWidget.client_rect().extents().cy - 1) }, prng(255),
						ng::pen{ random_colour(), prng(1, 3) });
					break;
				case 1:
					aGc.draw_circle(
						ng::point{ prng(circlesWidget.client_rect().cx - 1), prng(circlesWidget.client_rect().cy - 1) }, prng(255),
						ng::pen{ random_colour(), prng(1, 3) },
						random_colour().with_alpha(rngColour(255)));
					break;
				case 2:
					aGc.fill_circle(
						ng::point{ prng(circlesWidget.client_rect().cx - 1), prng(circlesWidget.client_rect().cy - 1) }, prng(255),
						random_colour().with_alpha(rngColour(255)));
					break;
				}
			}
		});
		tabContainer.add_tab_page("Foo").tab().set_image(smallHash);
		tabContainer.add_tab_page("Bar").tab().set_image(smallHash);

		return app.exec();
	}
	catch (std::exception& e)
	{
		app.halt();
		std::cerr << "neogfx::app::exec: terminating with exception: " << e.what() << std::endl;
		app.surface_manager().display_error_message(app.name().empty() ? "Abnormal Program Termination" : "Abnormal Program Termination - " + app.name(), std::string("main: terminating with exception: ") + e.what());
		std::exit(EXIT_FAILURE);
	}
	catch (...)
	{
		app.halt();
		std::cerr << "neogfx::app::exec: terminating with unknown exception" << std::endl;
		app.surface_manager().display_error_message(app.name().empty() ? "Abnormal Program Termination" : "Abnormal Program Termination - " + app.name(), "main: terminating with unknown exception");
		std::exit(EXIT_FAILURE);
	}
}

