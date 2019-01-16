void Query::setBestSequence() {
  Cardinality originalCardinality = Cardinality(this);

   vector<Predicate*> predicates_ = predicates;
  vector<Predicate*> bestSequence;
  int64_t max = -1, cost = 0;

   for (Filter* f : filters) {
    originalCardinality.assess(f);
  }

   do {
    Cardinality cardinality = originalCardinality;
    cost = 0;
    for (Predicate* pred : predicates_) {
      cost += cardinality.assess(pred);
    }
    if (max == -1 || cost < max) {
      max = cost;
      bestSequence = predicates_;
    }
  } while (std::next_permutation(predicates_.begin(), predicates_.end()));

   // predicates = bestSequence;
}
