#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#pragma once
using namespace System;
using namespace System::Configuration;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#using <system.dll>
#using <system.configuration.dll>

namespace NewVision {
	/// <summary>
	/// Summary for AppSettings
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	//ref class TagSection : ConfigurationSection
 //   {
	//private:
	//	static ConfigurationProperty^ _Tag = gcnew ConfigurationProperty("tag", Type::GetType("String"), "", ConfigurationPropertyOptions::IsRequired);
	//	static ConfigurationPropertyCollection^ _Properties;

	//public:
	//	TagSection() {
	//		_Properties = gcnew ConfigurationPropertyCollection();
	//		_Properties->Add(_Tag);
	//	}
	//	
	//	property ConfigurationPropertyCollection^ Properties
	//	{
	//		virtual ConfigurationPropertyCollection^ get() override	{
	//			return _Properties;
	//		}
	//	}

	//	property String^ Tag {
	//		String^ get() {
	//			return (String^)this["tag"];
	//		}
	//		void set(String^ value) {
	//			this["tag"] = value;
	//		}
	//	}
	//};
//// Read from congiguration file
			//System::Configuration::Configuration ^config = ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None);


			////// == Configuration Section: Pedestrian Tags
			//TagSection ^section = (TagSection ^)config->Sections["TagSection"];
			//if (!section)
			//{
			//	section = gcnew TagSection();
			//	config->Sections->Add("TagSection", section);
			//	section->SectionInformation->ForceSave = true;
			//}
			//else {
			//}
			////// == Configuration Section: Other

			//if (!section->ElementInformation->IsLocked)	
			//	config->Save(ConfigurationSaveMode::Full);
			//else
			//	System::Diagnostics::Debug::WriteLine("Section was locked, could not update.");
			

	public ref class AppSettings : public System::Windows::Forms::Form
	{
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~AppSettings()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl^  tabControl1;
	protected: 
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::ComboBox^  tags;
	private: System::Windows::Forms::ComboBox^  values;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  removeTag;
	private: System::Windows::Forms::Button^  addTag;
	private: System::Windows::Forms::Button^  removeValue;
	private: System::Windows::Forms::Button^  addValue;

	private: System::Windows::Forms::Button^  buttonOk;
	private: System::Windows::Forms::Button^  buttonCancel;
	private: System::Windows::Forms::Button^  buttonApply;
	private: BindingList<String^> ^dataTags;
	private: BindingList<String^> ^docSettings;
	private: BindingList<String^> ^dataValues;
	private: System::Configuration::KeyValueConfigurationCollection ^settings;
	private: System::Windows::Forms::TextBox^  textBoxAutoSave;
	private: System::Windows::Forms::CheckBox^  checkBoxAutoSave;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
	private: System::Windows::Forms::LinkLabel^  linChooseFolder;
	private: System::Configuration::Configuration ^XML;
	private: 
		void LoadBindingListFromXML(BindingList<String^> ^data, String ^tag) {
			KeyValueConfigurationElement ^myElem = settings[tag];
			if (!myElem)
				myElem = gcnew KeyValueConfigurationElement(tag, "");
			CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
			CommaDelimitedStringCollection ^myColl = (CommaDelimitedStringCollection ^)cv->ConvertFromString(myElem->Value);
			StringEnumerator^ myEnum = myColl->GetEnumerator();
			data->Clear();
			while ( myEnum->MoveNext() )
				data->Add(myEnum->Current);
		}
		void SaveBindingListToXML(BindingList<String^> ^data, String ^tag) {
			CommaDelimitedStringCollection ^myColl = gcnew CommaDelimitedStringCollection();
			for (int i=0; i<data->Count; i++)
				myColl->Add(data[i]);
			CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
			String ^myString = cv->ConvertToString(myColl);
			if (settings[tag]) 
				settings[tag]->Value = myString;
			else
				settings->Add(tag, myString);
		}
	public:

		AppSettings(void)
		{
			InitializeComponent();
			dataTags = gcnew BindingList<String ^>;
			tags->DataSource = dataTags;
			dataValues = gcnew BindingList<String ^>;
			values->DataSource = dataValues;
			this->textBoxAutoSave->TextChanged += gcnew EventHandler(this, &AppSettings::OnAutoSaveChanged);
			this->checkBoxAutoSave->CheckedChanged += gcnew EventHandler(this, &AppSettings::OnAutoSaveChanged);
		}
	protected: 
		virtual void OnShown (EventArgs^ e) override {
			Form::OnShown(e);
			XML = ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None);
			settings = XML->AppSettings->Settings;
			// load documents settings
			if (settings["autosave"])
				checkBoxAutoSave->Checked.Parse(settings["autosave"]->Value);
			else
				checkBoxAutoSave->Checked = false;
			if (settings["autosave interval"])
				textBoxAutoSave->Text = settings["autosave interval"]->Value;
			else
				textBoxAutoSave->Text = "300";
			if (settings["autosave folder"])
				linChooseFolder->Text = settings["autosave folder"]->Value;
			else
				linChooseFolder->Text = "Choose folder...";

			// load tags settings
			LoadBindingListFromXML(dataTags, "tags");
			if (dataTags->Count)
				LoadBindingListFromXML(dataValues, dataTags[0]+" values");
	}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->removeValue = (gcnew System::Windows::Forms::Button());
			this->addValue = (gcnew System::Windows::Forms::Button());
			this->removeTag = (gcnew System::Windows::Forms::Button());
			this->addTag = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->values = (gcnew System::Windows::Forms::ComboBox());
			this->tags = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->textBoxAutoSave = (gcnew System::Windows::Forms::TextBox());
			this->checkBoxAutoSave = (gcnew System::Windows::Forms::CheckBox());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->buttonApply = (gcnew System::Windows::Forms::Button());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->linChooseFolder = (gcnew System::Windows::Forms::LinkLabel());
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Location = System::Drawing::Point(1, 1);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(790, 279);
			this->tabControl1->TabIndex = 0;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->removeValue);
			this->tabPage1->Controls->Add(this->addValue);
			this->tabPage1->Controls->Add(this->removeTag);
			this->tabPage1->Controls->Add(this->addTag);
			this->tabPage1->Controls->Add(this->label2);
			this->tabPage1->Controls->Add(this->label1);
			this->tabPage1->Controls->Add(this->values);
			this->tabPage1->Controls->Add(this->tags);
			this->tabPage1->ForeColor = System::Drawing::SystemColors::Highlight;
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(782, 253);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Pedestrian Tags";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// removeValue
			// 
			this->removeValue->Location = System::Drawing::Point(418, 51);
			this->removeValue->Name = L"removeValue";
			this->removeValue->Size = System::Drawing::Size(57, 23);
			this->removeValue->TabIndex = 7;
			this->removeValue->Text = L"Remove";
			this->removeValue->UseVisualStyleBackColor = true;
			this->removeValue->Click += gcnew System::EventHandler(this, &AppSettings::removeValue_Click);
			// 
			// addValue
			// 
			this->addValue->Location = System::Drawing::Point(353, 51);
			this->addValue->Name = L"addValue";
			this->addValue->Size = System::Drawing::Size(59, 23);
			this->addValue->TabIndex = 6;
			this->addValue->Text = L"Add";
			this->addValue->UseVisualStyleBackColor = true;
			this->addValue->Click += gcnew System::EventHandler(this, &AppSettings::addValue_Click);
			// 
			// removeTag
			// 
			this->removeTag->Location = System::Drawing::Point(130, 51);
			this->removeTag->Name = L"removeTag";
			this->removeTag->Size = System::Drawing::Size(57, 23);
			this->removeTag->TabIndex = 5;
			this->removeTag->Text = L"Remove";
			this->removeTag->UseVisualStyleBackColor = true;
			this->removeTag->Click += gcnew System::EventHandler(this, &AppSettings::removeTag_Click);
			// 
			// addTag
			// 
			this->addTag->Location = System::Drawing::Point(65, 51);
			this->addTag->Name = L"addTag";
			this->addTag->Size = System::Drawing::Size(59, 23);
			this->addTag->TabIndex = 4;
			this->addTag->Text = L"Add";
			this->addTag->UseVisualStyleBackColor = true;
			this->addTag->Click += gcnew System::EventHandler(this, &AppSettings::addTag_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(351, 29);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(115, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Values for selected tag";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(63, 29);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(31, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Tags";
			// 
			// values
			// 
			this->values->FormattingEnabled = true;
			this->values->Location = System::Drawing::Point(354, 79);
			this->values->Name = L"values";
			this->values->Size = System::Drawing::Size(121, 21);
			this->values->TabIndex = 1;
			// 
			// tags
			// 
			this->tags->FormattingEnabled = true;
			this->tags->Location = System::Drawing::Point(66, 80);
			this->tags->Name = L"tags";
			this->tags->Size = System::Drawing::Size(121, 21);
			this->tags->TabIndex = 0;
			this->tags->SelectedIndexChanged += gcnew System::EventHandler(this, &AppSettings::tags_SelectedIndexChanged);
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->linChooseFolder);
			this->tabPage2->Controls->Add(this->textBoxAutoSave);
			this->tabPage2->Controls->Add(this->checkBoxAutoSave);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(782, 253);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Document Settings";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// textBoxAutoSave
			// 
			this->textBoxAutoSave->Location = System::Drawing::Point(180, 4);
			this->textBoxAutoSave->Name = L"textBoxAutoSave";
			this->textBoxAutoSave->Size = System::Drawing::Size(63, 20);
			this->textBoxAutoSave->TabIndex = 1;
			this->textBoxAutoSave->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// checkBoxAutoSave
			// 
			this->checkBoxAutoSave->AutoSize = true;
			this->checkBoxAutoSave->Checked = true;
			this->checkBoxAutoSave->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxAutoSave->Location = System::Drawing::Point(7, 6);
			this->checkBoxAutoSave->Name = L"checkBoxAutoSave";
			this->checkBoxAutoSave->Size = System::Drawing::Size(167, 17);
			this->checkBoxAutoSave->TabIndex = 0;
			this->checkBoxAutoSave->Text = L"Autosave Date Every (frames)";
			this->checkBoxAutoSave->UseVisualStyleBackColor = true;
			// 
			// buttonOk
			// 
			this->buttonOk->Location = System::Drawing::Point(582, 291);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(94, 28);
			this->buttonOk->TabIndex = 2;
			this->buttonOk->Text = L"Ok";
			this->buttonOk->UseVisualStyleBackColor = true;
			this->buttonOk->Click += gcnew System::EventHandler(this, &AppSettings::buttonOk_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(683, 291);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(94, 28);
			this->buttonCancel->TabIndex = 3;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// buttonApply
			// 
			this->buttonApply->Location = System::Drawing::Point(12, 291);
			this->buttonApply->Name = L"buttonApply";
			this->buttonApply->Size = System::Drawing::Size(94, 28);
			this->buttonApply->TabIndex = 1;
			this->buttonApply->Text = L"Apply";
			this->buttonApply->UseVisualStyleBackColor = true;
			this->buttonApply->Click += gcnew System::EventHandler(this, &AppSettings::buttonApply_Click);
			// 
			// folderBrowserDialog1
			// 
			this->folderBrowserDialog1->RootFolder = System::Environment::SpecialFolder::MyComputer;
			// 
			// linChooseFolder
			// 
			this->linChooseFolder->AutoSize = true;
			this->linChooseFolder->Location = System::Drawing::Point(258, 7);
			this->linChooseFolder->Name = L"linChooseFolder";
			this->linChooseFolder->Size = System::Drawing::Size(81, 13);
			this->linChooseFolder->TabIndex = 2;
			this->linChooseFolder->TabStop = true;
			this->linChooseFolder->Text = L"Choose folder...";
			this->linChooseFolder->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AppSettings::linChooseFolder_LinkClicked);
			// 
			// AppSettings
			// 
			this->AcceptButton = this->buttonApply;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::MenuBar;
			this->CancelButton = this->buttonCancel;
			this->ClientSize = System::Drawing::Size(789, 338);
			this->Controls->Add(this->buttonApply);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->tabControl1);
			this->ForeColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = L"AppSettings";
			this->Text = L"Application Settings";
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void OnAutoSaveChanged(System::Object^  sender, System::EventArgs^  e) {
		 // load documents settings
		 settings["autosave"]->Value =  checkBoxAutoSave->Checked.ToString();
		 settings["autosave interval"]->Value = textBoxAutoSave->Text;
	}

	private: System::Void addTag_Click(System::Object^  sender, System::EventArgs^  e) {
		String ^text=tags->Text;
		dataTags->Add(text);
		SaveBindingListToXML(dataTags, "tags");
		tags->SelectedItem = text;
	}
	private: System::Void removeTag_Click(System::Object^  sender, System::EventArgs^  e) {
		if (tags->SelectedIndex == -1)
			return;
		settings->Remove(tags->SelectedItem->ToString()+" values");
		dataTags->Remove(tags->SelectedItem->ToString());
		SaveBindingListToXML(dataTags, "tags");
	}
	private: System::Void addValue_Click(System::Object^  sender, System::EventArgs^  e) {
		String ^text=values->Text;
		dataValues->Add(text);
		SaveBindingListToXML(dataValues, dataTags[tags->SelectedIndex]+" values");
		values->SelectedItem = text;
	}
	private: System::Void removeValue_Click(System::Object^  sender, System::EventArgs^  e) {
		if (values->SelectedIndex == -1)
			return;
		dataValues->Remove(tags->SelectedItem->ToString());
		SaveBindingListToXML(dataValues, dataTags[tags->SelectedIndex]+" values");
	}
	private: System::Void buttonApply_Click(System::Object^  sender, System::EventArgs^  e) {
		XML->Save();
	}
	private: System::Void buttonOk_Click(System::Object^  sender, System::EventArgs^  e) {
		XML->Save();
		Close();
	}
	private: System::Void tags_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		if (tags->SelectedIndex == -1)
			dataValues->Clear();
		else
			LoadBindingListFromXML(dataValues, dataTags[tags->SelectedIndex]+" values");
	}
	private: System::Void linChooseFolder_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
			::DialogResult result = folderBrowserDialog1->ShowDialog();
			
			// OK button was pressed.
				 if ( result == ::DialogResult::OK )
			{
				linChooseFolder->Text = folderBrowserDialog1->SelectedPath;
				settings["autosave folder"]->Value = linChooseFolder->Text;
			}
			// Cancel button was pressed.
			else {
			}
	}
};
}

#endif // !defined( APP_SETTINGS_H )