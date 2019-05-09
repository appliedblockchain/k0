FROM appliedblockchain/snarkapps-base

COPY --from=zktrading /project/build/src/setup /setup

ENTRYPOINT ["/setup"]