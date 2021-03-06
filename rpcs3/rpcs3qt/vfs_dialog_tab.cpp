#include "vfs_dialog_tab.h"

#include <QFileDialog>
#include <QCoreApplication>

inline std::string sstr(const QString& _in) { return _in.toStdString(); }

vfs_dialog_tab::vfs_dialog_tab(const vfs_settings_info& settingsInfo, std::shared_ptr<gui_settings> guiSettings, std::shared_ptr<emu_settings> emuSettings, QWidget* parent)
	: QWidget(parent), m_info(settingsInfo), m_gui_settings(guiSettings), m_emu_settings(emuSettings)
{
	m_dirList = new QListWidget(this);

	QStringList alldirs = m_gui_settings->GetValue(m_info.listLocation).toStringList();
	const QString current_dir = qstr(m_info.cfg_node->to_string());

	QListWidgetItem* selected_item = nullptr;

	for (const QString& dir : alldirs)
	{
		QListWidgetItem* item = new QListWidgetItem(dir, m_dirList);
		if (dir == current_dir)
			selected_item = item;
	}

	// We must show the currently selected config.
	if (!selected_item)
		selected_item = new QListWidgetItem(current_dir, m_dirList);

	selected_item->setSelected(true);

	m_dirList->setMinimumWidth(m_dirList->sizeHintForColumn(0));

	QHBoxLayout* selectedConfigLayout = new QHBoxLayout;
	QLabel* selectedMessage = new QLabel(m_info.name + " directory:");
	m_selectedConfigLabel = new QLabel();
	m_selectedConfigLabel->setText(current_dir.isEmpty() ? EmptyPath : current_dir);
	selectedConfigLayout->addWidget(selectedMessage);
	selectedConfigLayout->addWidget(m_selectedConfigLabel);
	selectedConfigLayout->addStretch();

	QVBoxLayout* vbox = new QVBoxLayout;
	vbox->addWidget(m_dirList);
	vbox->addLayout(selectedConfigLayout);

	setLayout(vbox);

	connect(m_dirList, &QListWidget::currentItemChanged, [this](QListWidgetItem* current, QListWidgetItem*)
	{
		m_selectedConfigLabel->setText(current->text().isEmpty() ? EmptyPath : current->text());
	});
}

void vfs_dialog_tab::SetSettings()
{
	QStringList allDirs;
	for (int i = 0; i < m_dirList->count(); ++i)
	{
		allDirs += m_dirList->item(i)->text();
	}
	m_gui_settings->SetValue(m_info.listLocation, allDirs);

	const std::string new_dir = m_selectedConfigLabel->text() == EmptyPath ? "" : sstr(m_selectedConfigLabel->text());
	m_info.cfg_node->from_string(new_dir);
	m_emu_settings->SetSetting(m_info.settingLoc, new_dir);
}

void vfs_dialog_tab::Reset()
{
	const QString current_dir = qstr(m_info.cfg_node->to_string());
	m_dirList->clear();
	m_info.cfg_node->from_default();
	m_selectedConfigLabel->setText(current_dir.isEmpty() ? EmptyPath : current_dir);
	m_dirList->addItem(new QListWidgetItem(current_dir));
	m_gui_settings->SetValue(m_info.listLocation, QStringList(current_dir));
}

void vfs_dialog_tab::AddNewDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(nullptr, "Choose a directory", QCoreApplication::applicationDirPath());
	if (dir != "")
	{
		if (dir.endsWith("/") == false) dir += '/';
		new QListWidgetItem(dir, m_dirList);
		m_selectedConfigLabel->setText(dir);
	}
}
