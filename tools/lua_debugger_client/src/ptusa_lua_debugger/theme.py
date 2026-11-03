"""Compact desktop controls layered over the Material palette."""

from PySide6.QtWidgets import QApplication
from qt_material import apply_stylesheet


def apply_theme(application: QApplication) -> None:
    apply_stylesheet(application, theme="dark_teal.xml")
    application.setStyleSheet(application.styleSheet() + """
        QWidget { font-family: 'Segoe UI'; font-size: 12px; }
        QPushButton {
            min-width: 0px; height: 20px; min-height: 20px; padding: 4px 10px;
            border: 1px solid #526068; border-radius: 4px;
            background-color: #343d43; color: #e0e6e9;
            text-transform: none; font-weight: 400;
        }
        QPushButton:hover { background-color: #414d54; border-color: #82949e; }
        QPushButton:pressed { background-color: #253037; }
        QPushButton[primary="true"] {
            background-color: #00bfa5; border-color: #00bfa5;
            color: #102c29; font-weight: 600;
        }
        QPushButton[primary="true"]:hover { background-color: #35d5bd; }
        QPushButton[primary="true"]:pressed { background-color: #009c88; }
        QPushButton:disabled {
            background-color: #2d3439; border-color: #414b51; color: #76858e;
        }
        QPushButton:focus { border: 1px solid #a3f5e7; }
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            height: 20px; min-height: 20px; padding: 4px 8px;
            border: 1px solid #4a555d; border-radius: 4px;
            background-color: #252c31;
        }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
            border: 1px solid #00bfa5;
        }
        QCheckBox { spacing: 6px; }
        QCheckBox::indicator { width: 15px; height: 15px; }
        QTabBar::tab { min-height: 20px; padding: 5px 12px; text-transform: none; }
        QHeaderView::section {
            padding: 7px 8px; text-transform: none; font-weight: 600;
        }
        QTreeView::item { padding: 3px 0px; }
        QSplitter::handle { background-color: #30383e; }
        QSplitter::handle:hover { background-color: #00bfa5; }
        QLabel#sessionStatus { color: #aab9c2; padding-top: 3px; }
    """)
