from setuptools import setup, find_packages



setup(
  name="pyitalonet",
  version="0.0.1",
  license="ZLIB",
  author="jeff",
  author_email="jeff@i2p.rocks",
  description="italonet python bindings",
  url="https://github.com/italo-project/italo-network",
  install_requires=["pysodium", "requests", "python-dateutil"],
  packages=find_packages())