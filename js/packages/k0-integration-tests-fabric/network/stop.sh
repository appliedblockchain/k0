if [ "$CI" = "true" ]
then
  maybe_ci_dc_file="-f docker-compose-ci.yaml"
fi

docker-compose -f docker-compose.yaml $maybe_ci_dc_file down
