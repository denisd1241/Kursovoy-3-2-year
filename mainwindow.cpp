#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLabel>
#include <QGridLayout>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QInputDialog>
#include <QQueue> // Add this for tree layout

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gridLayoutWidget->setStyleSheet("background-color: #a8a8a8; border: 1px solid #333;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVector<QVector<int>> createSymmetricMatrix(int size) {
    srand(time(0));

    QVector<QVector<int>> matrix(size, QVector<int>(size, 0));

    for (int i = 0; i < size; ++i) {
        for (int j = i; j < size; ++j) {
            int randomValue = rand() % 2;
            matrix[i][j] = randomValue;
            matrix[j][i] = randomValue;
        }
    }

    return matrix;
}

void MainWindow::on_pushButton_clicked(){
    QGridLayout *gridLayout = ui->gridLayout;

    bool ok;
    int matrixSize = QInputDialog::getInt(this, tr("Введите размер матрицы"), tr("Размер матрицы:"), 3, 1, 100, 1, &ok);

    if (ok) {
        clearGridLayout(gridLayout);
        QVector<QVector<int>> symmetricMatrix = createSymmetricMatrix(matrixSize);

        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                QLabel *label = new QLabel(QString::number(symmetricMatrix[i][j]));
                label->setStyleSheet("border: none;");
                label->setAlignment(Qt::AlignCenter);
                label->setMinimumSize(30, 30);
                label->setMaximumSize(30, 30);
                gridLayout->addWidget(label, i, j);
            }
        }

        // Рисуем граф
        drawGraph(symmetricMatrix);
    }
}

void MainWindow::clearGridLayout(QGridLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

void MainWindow::drawGraph(const QVector<QVector<int>>& matrix)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    int nodeRadius = 20;
    int xOffset = 50;
    int yOffset = 50;

    QVector<QPoint> nodePositions(matrix.size());

    // Calculate positions for nodes in a top-down tree layout
    for (int i = 0; i < matrix.size(); ++i) {
        int x = i * xOffset;
        int y = 0;

        // Find the maximum y-coordinate among adjacent nodes
        for (int j = 0; j < i; ++j) {
            if (matrix[j][i] > 0) {
                y = std::max(y, nodePositions[j].y() + yOffset);
            }
        }

        nodePositions[i] = QPoint(x, y);

        QGraphicsEllipseItem *node = new QGraphicsEllipseItem(x, y, nodeRadius, nodeRadius);
        scene->addItem(node);

        // Add labels with vertex numbers
        QGraphicsTextItem *label = new QGraphicsTextItem(QString::number(i + 1));
        label->setPos(x + nodeRadius / 2 - 5, y + nodeRadius / 2 - 5);
        scene->addItem(label);
    }

    // Draw edges based on the adjacency matrix
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = i; j < matrix.size(); ++j) {
            int weight = matrix[i][j];
            if (weight > 0) {
                // Draw multiple lines for each edge
                for (int k = 0; k < weight; ++k) {
                    // Draw a line between nodes
                    scene->addLine(nodePositions[i].x() + nodeRadius / 2, nodePositions[i].y() + nodeRadius / 2,
                                   nodePositions[j].x() + nodeRadius / 2, nodePositions[j].y() + nodeRadius / 2);
                }
            }
        }
    }
}
