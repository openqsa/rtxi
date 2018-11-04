/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 *
 * Copyright (C) 2018 OpenQSA. Quadratic Sinusoidal Analysis.
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */

#include "qsa_response.h"
#include <main_window.h>

#include <fstream>
#include <iostream>

extern "C"
Plugin::Object * createRTXIPlugin()
{
        return new QsaResponse();
}

namespace
{
DefaultGUIModel::variable_t vars[] =
{
        { "Iqsa", "current", DefaultGUIModel::INPUT},
        { "Vm", "membrane potential", DefaultGUIModel::INPUT},
        { "Sync", "synchronization", DefaultGUIModel::INPUT}
};

std::size_t num_vars = sizeof (vars) / sizeof (DefaultGUIModel::variable_t);
}

QsaResponse::QsaResponse()
:
        DefaultGUIModel("QsaResponse with Custom GUI", ::vars, ::num_vars)
{
        setWhatsThis(
                "<p><b>QsaResponse:</b>"
                "<br>Record response to QSA stimulation</p>");
        DefaultGUIModel::createGUI(vars, num_vars);
        customizeGUI();
        initParameters();

        update(INIT);
        refresh();
        QTimer::singleShot(0, this, SLOT(resizeMe()));
}

QsaResponse::~QsaResponse()
{
}

void QsaResponse::execute()
{
        if (recording)
        {
                recorder.push(
                        input(indexIqsa),
                        input(indexVm),
                        input(indexSync));
                if (recorder.started() && recorder.stopped())
                {
                        recording = false;
                        recordButton->setEnabled(true);
                        cancelButton->setEnabled(false);
                        saveButton->setEnabled(true);
                }
        }
}

void QsaResponse::initParameters()
{
}

void QsaResponse::doModify()
{
        // Set input indices
        auto assign = [&](const std::string & name, std::size_t & index)
        {
                for (std::size_t i = 0; i < 3; i++)
                {
                        if (getName(INPUT, i) == name)
                        {
                                index = i;
                                break;
                        } 
                }
        };
        assign("Iqsa", indexIqsa);
        assign("Vm", indexVm);
        assign("Sync", indexSync);
        recordButton->setEnabled(true);
}

void QsaResponse::update(DefaultGUIModel::update_flags_t flag)
{
        switch (flag)
        {
        case INIT:
        {
                period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
                break;
        }

        case MODIFY:
        {
                doModify();
                break;
        }

        case UNPAUSE:
                break;

        case PAUSE:
                break;

        case PERIOD:
        {
                period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
                break;
        }

        default:
                break;
        }
}

void QsaResponse::customizeGUI()
{
        // Paste button
        QPushButton * pasteButton = new QPushButton("Paste stimulation");
        QObject::connect(
                pasteButton,
                SIGNAL(clicked()),
                this,
                SLOT(onClickPasteButton()));

        // Paste group
        QGroupBox * pasteGroup = new QGroupBox;
        QHBoxLayout* pasteLayout = new QHBoxLayout;
        pasteGroup->setLayout(pasteLayout);
        pasteLayout->addWidget(pasteButton);

        // Recorder buttons
        recordButton = new QPushButton("Record sequence");
        recordButton->setEnabled(false);
        QObject::connect(
                recordButton,
                SIGNAL(clicked()),
                this,
                SLOT(onClickRecordButton()));
        cancelButton = new QPushButton("Cancel");
        cancelButton->setEnabled(false);
        QObject::connect(
                cancelButton,
                SIGNAL(clicked()),
                this,
                SLOT(onClickCancelButton()));
        saveButton = new QPushButton("Save to disk");
        saveButton->setEnabled(false);
        QObject::connect(
                saveButton,
                SIGNAL(clicked()),
                this,
                SLOT(onClickSaveButton()));

        // Recorder group
        auto recorderGroup = new QGroupBox;
        auto recorderLayout = new QHBoxLayout;
        recorderGroup->setLayout(recorderLayout);
        recorderLayout->addWidget(recordButton);
        recorderLayout->addWidget(cancelButton);
        recorderLayout->addWidget(saveButton);

        // Text edit
        stimulationEdit = new QTextEdit;
        stimulationEdit->setReadOnly(true);

        // Custom layout
        QGridLayout * customlayout = DefaultGUIModel::getLayout();
        customlayout->addWidget(pasteGroup);
        customlayout->addWidget(recorderGroup);
        customlayout->addWidget(stimulationEdit);
        setLayout(customlayout);
}

void QsaResponse::onClickPasteButton()
{
        auto cb = QApplication::clipboard();
        auto encoding = cb->text().toStdString();
        auto stimulation = Qsa::StimulationConverter::parse(encoding);
        recorder.set_stimulation(stimulation);
        auto text = QString::fromStdString(stimulation.to_string());
        stimulationEdit->setText(text);
}

void QsaResponse::onClickRecordButton()
{
        recordButton->setEnabled(false);
        cancelButton->setEnabled(true);
        saveButton->setEnabled(false);
        recorder.start();
        recording = true;
}

void QsaResponse::onClickSaveButton()
{
        QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Save File"),
                "",
                tr("Text Files (*.txt)"));
        if (filename != "")
                recorder.save(filename.toStdString());
}

void QsaResponse::onClickCancelButton()
{
        recorder.stop();
        cancelButton->setEnabled(false);
        recordButton->setEnabled(true);
}
