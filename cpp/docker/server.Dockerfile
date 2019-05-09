FROM appliedblockchain/snarkapps-base

COPY --from=zktrading /project/build/src/server /server

ENTRYPOINT ["/server"]