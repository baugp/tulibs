remake_add_documentation(
  DOXYGEN Doxyfile.remake
)
remake_add_documentation(
  TARGETS lsusb lsftdi spline_evaluate
  ARGS --man-output=%OUTPUT%
    --man-title="${REMAKE_PROJECT_NAME} Utilities Documentation"
    --man-project-name="${REMAKE_PROJECT_NAME}"
    --man-project-version="${REMAKE_PROJECT_VERSION}"
    --man-project-authors="${REMAKE_PROJECT_AUTHORS}"
    --man-project-contact="${REMAKE_PROJECT_CONTACT}"
    --man-project-home="${REMAKE_PROJECT_HOME}"
    --man-project-license="${REMAKE_PROJECT_LICENSE}"
  OUTPUT_FILE man1/%EXECUTABLE%.1.gz
  MAKE_DIRECTORIES
  TYPES man
  COMPONENT utils
)
