#ifndef ONLINE_RANDOM_FOREST_CLASSIFIER_H
#define ONLINE_RANDOM_FOREST_CLASSIFIER_H

#include <classification_tree_node.h>
#include <sample.h>
#include <oob_error.h>

#include <memory>
#include <vector>
#include <cstdlib>



template <class T, class NodeType>
class OnlineRandomForestClassifier{
  typedef pair<unique_ptr<NodeType>,unique_ptr<oob_error<T>>> TreeNodeOOB;

  public:
  
  int number_of_trees;
  int number_of_features;
  int number_of_decision_functions;
  int min_samples_to_split;
  int max_samples_to_hold;
  int max_tree_depth;

  unique_ptr<vector<TreeNodeOOB>> trees;

  
  OnlineRandomForestClassifier(
    int number_of_trees,
    int number_of_features,
    int number_of_decision_functions,
    int min_samples_to_split,
    int max_samples_to_hold,
    int max_tree_depth
  ):
    number_of_trees(number_of_trees),
    number_of_features(number_of_features),
    number_of_decision_functions(number_of_decision_functions),
    min_samples_to_split(min_samples_to_split),
    max_samples_to_hold(max_samples_to_hold),
    max_tree_depth(max_tree_depth)
  {
    trees.reset(new vector<TreeNodeOOB>());
    for(int i=0;i<number_of_trees;i++){
      vector<shared_ptr<Sample<T>>> initial_samples=vector<shared_ptr<Sample<T>>>();
      trees->push_back(
        TreeNodeOOB(
          unique_ptr<NodeType>(new NodeType(
            number_of_features,
            number_of_decision_functions,
            min_samples_to_split,
            max_samples_to_hold,
            max_tree_depth,
            initial_samples
          )),
          unique_ptr<oob_error<T>>(new oob_error<T>(0.,0.))
        ));
    }
    
  }

  ~OnlineRandomForestClassifier(){}

  void update_oob_error(TreeNodeOOB& tree, shared_ptr<Sample<T>> sample){
    T prediction=tree.predict(sample->features);
    if(prediction!=sample->prediction){
      tree.second->oob_absolute_error+=1;
    }
    tree.second->oob_samples_count+=1;
  }


  void update_tree(TreeNodeOOB& tree, shared_ptr<Sample<T>> sample){
    int k=rand()%2;
    for(int i=0;i<k;i++){
      tree.update(sample);
    }
    if(k==0){
      update_oob_error(tree, sample);
    }
  }

};


#endif //ONLINE_RANDOM_FOREST_CLASSIFIER_H



