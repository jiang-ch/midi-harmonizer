#include<stdio.h>
#include<stdlib.h>
#include"../NoteSynth.c"

typedef struct BST_Node_Struct
{
    double key;
    double freq;
    int bar;
    double index;
    struct BST_Node_Struct *left;
    struct BST_Node_Struct *right;
} BST_Node;

BST_Node *newBST_Node(double freq, int bar, double index)
{
    /* 
     * This function creates and initializes a new BST_Node 
     * for a note with the given position (bar:index) and
     * the specified frequency. The key value for the node
     * is computed here as
     * 
     * 		(10.0*bar)+index
     */
  BST_Node *new=(BST_Node *)calloc(1,sizeof(BST_Node));
  new->freq=freq;
  new->bar=bar;
  new->index=index;
  new->key=bar*10.0+index;
  new->left=NULL;
  new->right=NULL;
  return new;
}

BST_Node *BST_insert(BST_Node *root, BST_Node *new_node)
{
    /*
     * This function inserts a new node into the BST. The
     * node must already have been initialized with valid
     * note data, and must have its unique key.
     */
  if (root==NULL) return new_node;
  if (new_node->key==root->key){
    printf("Duplicate node requested (bar:index)=%d,%lf, it was ignored\n",new_node->bar,new_node->index); 
    return root;
  }
  else if (new_node->key<root->key) root->left=BST_insert(root->left,new_node);
  else root->right=BST_insert(root->right,new_node);
  return root;
}

BST_Node *BST_search(BST_Node *root, int bar, double index)
{
    /*
     * This function searches the BST for a note at the
     * speficied position. If found, it must return a
     * pointer to the node that contains it. 
     */
    
  if (root==NULL) return NULL;
  if (root->key==10*bar+index) return root; 
  else if (10*bar+index<root->key)  
    return BST_search(root->left,bar,index);
  else
    return BST_search(root->right,bar,index);
}

BST_Node *find_successor(BST_Node *right_child_node)
{
    /*
     * This function finds the successor of a node by 
     * searching the right subtree for the node that
     * is most to the left (that will be the node
     * with the smallest key in that subtree)
     */
  
  while (right_child_node->left) {
    right_child_node=right_child_node->left;
  }
  return right_child_node;
    
}

BST_Node *BST_delete(BST_Node *root, int bar, double index)
{
    /*
     * Deletes from the BST a note at the specified position.
     */
  if (root==NULL) return NULL;
  BST_Node *tran=root;
  if (tran->key==bar*10+index){   
    // the node has no children
    if (tran->left==NULL && tran->right==NULL){ 
      free(tran);
      return NULL;
    }
    // the node has one child
    else if (tran->right==NULL) {
      root=tran->left;
      free(tran);
      return root;
    }
    else if (tran->left==NULL) {
      root=tran->right;
      free(tran);
      return root;
    }
    // the node has two children
    else {
      BST_Node *succ=find_successor(tran->right);
      tran->bar=succ->bar;
      tran->freq=succ->freq;
      tran->index=succ->index;
      tran->key=succ->key;
      tran->right=BST_delete(tran->right,succ->bar,succ->index);
      return root;
    }
  }
  else if (tran->key>bar*10+index) {
    tran->left=BST_delete(tran->left,bar,index);
    return root;
  }
  else {
    tran->right=BST_delete(tran->right,bar,index);
    return root;
  }
    
}

void BST_makePlayList(BST_Node *root)
{
    /*
     * This function does an in-order traversal of the BST to
     * generate an ordered list of notes to be played. Each
     * note is added to a linked-list
     */
  if (root==NULL) return;
  BST_makePlayList(root->left);
  playlist_head=playlist_insert(playlist_head,root->freq,root->bar,root->index);
  BST_makePlayList(root->right);
}

void BST_inOrder(BST_Node *root, int depth)
{
    /*
     * This function performs an in-order traversal of the BST
     * and prints out the note information for each note
     */
  if (root==NULL) return;
  BST_inOrder(root->left,depth+1);
  printf("Depth=%d, Bar:Index (%d:%f), F=%f Hz\n",depth,root->bar,root->index,root->freq);
  BST_inOrder(root->right,depth+1); 
} 

void BST_preOrder(BST_Node *root, int depth)
{
    /*
     * This function performs an pre-order traversal of the BST
     * and prints out the note information for each note
     */
  if (root==NULL) return;
  printf("Depth=%d, Bar:Index (%d:%f), F=%f Hz\n",depth,root->bar,root->index,root->freq);
  BST_preOrder(root->left,depth+1);
  BST_preOrder(root->right,depth+1);
}

void BST_postOrder(BST_Node *root,int depth)
{
    /*
     * This function performs an post-order traversal of the BST
     * and prints out the note information for each note
     */
    
  if (root==NULL) return;
  BST_postOrder(root->left,depth+1);
  BST_postOrder(root->right,depth+1);
  printf("Depth=%d, Bar:Index (%d:%f), F=%f Hz\n",depth,root->bar,root->index,root->freq);
} 

void delete_BST(BST_Node *root)
{
    /*
     * This function deletes the BST and frees all memory used for
     * nodes in it. 
     */
  if (root==NULL) return;
  delete_BST(root->left);
  delete_BST(root->right);
  free(root);
} 


void change_freq(BST_Node *root, double find, double change){
  if (root==NULL) return;
  if (fabs(root->freq-find)<0.00000001) root->freq=change;
  change_freq(root->left, find, change);
  change_freq(root->right, find, change);
}

void BST_shiftFreq(BST_Node *root, char note_src[5], char note_dst[5])
{
    /*
     * This function goes over the entries in the BST, finds notes
     * corresponding to note_src (which is a text string describing
     * a note's name in the musical scale), and shifts those notes
     * to the frequency of note_dst (another text string with a note
     * name). 
     * */
  double freq_find=0;
  double freq_change=0;
  for (int i = 0; i < 100; ++i) {
    if (strcmp(note_names[i],note_src)==0)
      freq_find=note_freq[i];
    if (strcmp(note_names[i],note_dst)==0)
      freq_change=note_freq[i];
    if (freq_find && freq_change) break;
  }
  change_freq(root,freq_find,freq_change);
}

BST_Node *record_new_note(BST_Node *root, BST_Node *record, int semitones, double time_shift){
  if (root==NULL) return record;
  int i;
  for (i=0; i <100 && note_freq[i]; ++i){
    if (fabs(note_freq[i]-root->freq)<0.00000001) break;
  }
  int fre_sum=i+semitones;
  double time_sum=root->index+time_shift;
  if (fre_sum<=99 && fre_sum>=0 && time_sum<1 && time_sum>=0){ 
    BST_Node *new=newBST_Node(note_freq[fre_sum],root->bar,time_sum);
    record=BST_insert(record,new);
  }
  record=record_new_note(root->left,record,semitones,time_shift);
  record=record_new_note(root->right,record,semitones,time_shift);
  return record;
}


BST_Node *insert_new_node(BST_Node *root, BST_Node *record){
  if (root==NULL || record==NULL) return root;
  root=insert_new_node(root,record->left);
  root=insert_new_node(root,record->right);
  record->left=NULL;
  record->right=NULL;
  root=BST_insert(root, record);
  return root;
}


BST_Node *BST_harmonize(BST_Node *root, int semitones, double time_shift)
{
    /* This function traverses the BST, and for each existing
     * note, inserts a new, modified note (i.e. it will add sounds
     * to an already existing song, based on the notes it already has)
     */
  BST_Node *record=NULL;
  record=record_new_note(root,record,semitones,time_shift);
  root=insert_new_node(root,record); 
  return root; 
}
