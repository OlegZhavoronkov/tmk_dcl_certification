#!/bin/sh

# @autor Alin42

# this script will download fox images
# you can use them as test data for savemiddle if you want to

if [ -e image.jpg ] || [ -e defects/segmap_22_IMG_9964214_crop.jpg ] || [ -e defects/segmap_34_IMG_9964217_crop.jpg ] || [ -e efects/segmap_42_IMG_9964219_crop.jpg ] || [ -e defects/segmap_58_IMG_996424_crop.jpg ] || [ -e defects/segmap_62_IMG_996425_crop.jpg ]
then
    echo "Looks like you have some images downloaded"
    echo "Skipping"
else
    wget https://bigpicture.ru/wp-content/uploads/2013/07/fox02.jpg -O image.jpg

    mkdir -p defects
    wget https://bigpicture.ru/wp-content/uploads/2013/07/fox10.jpg -O defects/segmap_22_IMG_9964214_crop.jpg
    wget https://bigpicture.ru/wp-content/uploads/2013/07/fox03.jpg -O defects/segmap_34_IMG_9964217_crop.jpg
    wget https://bigpicture.ru/wp-content/uploads/2013/07/fox05.jpg -O defects/segmap_42_IMG_9964219_crop.jpg
    wget https://bigpicture.ru/wp-content/uploads/2013/07/fox06.jpg -O defects/segmap_58_IMG_996424_crop.jpg
    wget https://bigpicture.ru/wp-content/uploads/2013/07/fox04.jpg -O defects/segmap_62_IMG_996425_crop.jpg
fi
