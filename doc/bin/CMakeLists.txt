remake_add_documentation(
  TARGETS lsusb lsftdi spline_eval
  ARGS --man-output=%OUTPUT%
    --man-title="${REMAKE_PROJECT_NAME} Utilities Documentation"
    --project-name="${REMAKE_PROJECT_NAME}"
    --project-version="${REMAKE_PROJECT_VERSION}"
    --project-authors="${REMAKE_PROJECT_AUTHORS}"
    --project-contact="${REMAKE_PROJECT_CONTACT}"
    --project-home="${REMAKE_PROJECT_HOME}"
    --project-license="${REMAKE_PROJECT_LICENSE}"
  OUTPUT_FILE man1/%EXECUTABLE%.1.gz
  MAKE_DIRECTORIES
  TYPES man
  COMPONENT utils
)
