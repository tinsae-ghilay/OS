#!/bin/bash


# by Tinsae Ghilay Kahsay on september 5, 2024


# Script copies data from one folder to another and rearanges directory hierarchy 
# Dir hierarchy is changed from  
# music
# - <Artist>
# - - <Year>
# - - - <Album>
# - - - - <Song>
# to 
# Music DiBSE
# - <Year>
# - - <Artist>
# - - - <Song>
# first lets unzip music archive if not unzipped already
if [[ ! -d ./music ]] ; then
    # NOTE using -qq for a quiter and n to skip if file already exists 
    unzip -qqn ./music.zip
    # just to add to rubustness we check for success of unzip
    # less than 2 because according to unzip man page 
    # 0 = success,
    # 1 = warnings occured but success and 
    # 2 = generic error with probable success using workarounds
    # beyond 2 are all failurs
    if [[ $? -lt 3 ]] ; then
        echo "Unzip success"
    else 
        # and we exit since there is nothing to be done if unzip is not successful
        exit 1
    fi
else
    echo "unzipped file already exists"
fi

# we create a target directory to copy all data from old directory
# calling it Music DiBSE (Specified)
# NOTE we use the -p to avoid error if directory exists.(@see https://man7.org/linux/man-pages/man1/mkdir.1.html)
target="./Music DiBSE"
mkdir -p "${target}"

# loop through each artist directory
for dir in ./music/* ; do

    # we check if file is a directory first to avoid errors
    if [[ -d "${dir}" ]];then

        # loop through each year directory
        for year in "${dir}"/* ; do 

            # the current artist name
            artist="$(basename "$dir")"

            # creating folders by year first NOTE -> we use -p argument to make directory as needed (avoid error if it exists)
            path="${target}/$(basename "$year")"
            mkdir -p  "${path}"

            # we create artist directory as subdirectory of year
            mkdir  -p "${path}/${artist}"

            # finaly we copy songs from album. NOTE-> we dont need Album (@see specifications in exercise)
            # NOTE -> we use -r argument to copy subfolders recursively (just incase) and -u to update instead of replace if file exists.
            cp -f "${year}"/*/* "${path}/${artist}"
        done # done copying folders
    fi
done
echo "Task complete"